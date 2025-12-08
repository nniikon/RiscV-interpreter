#include "rvi_read_binary.hpp"

#include "rvi_parse_elf.hpp"
#include <exception>
#include <cstring>
#include <stdexcept>

using namespace rvi;

ReadBinary::ReadBinary(std::string_view path)
    : path_(path),
      mmap_(path) {
}

ReadBinary::SectionInfo ReadBinary::GetTextSectionView() const {
    uint32_t text_offset  = 0u;
    uint32_t text_size    = 0u;
    uint32_t start_offset = 0u;
    if (!elf::GetTextSectionOffsetSize(path_.c_str(), &text_offset, &text_size, &start_offset)) {
        throw std::runtime_error("Can't find .text section");
    }

    return {mmap_.GetView().subspan(text_offset, text_size), start_offset};
}

void ReadBinary::LoadIntoMemory(InterpreterMemoryModel* memory, uint32_t* entry_point) const {
    auto view = mmap_.GetView();
    if (view.size() < sizeof(Elf32_Ehdr)) {
        throw std::runtime_error("ELF header is truncated");
    }

    const auto* eh = reinterpret_cast<const Elf32_Ehdr*>(view.data());

    if (std::memcmp(eh->e_ident, ELFMAG, SELFMAG) != 0) {
        throw std::runtime_error("Invalid ELF magic");
    }
    if (eh->e_ident[EI_CLASS] != ELFCLASS32 || eh->e_ident[EI_DATA] != ELFDATA2LSB) {
        throw std::runtime_error("Unsupported ELF format");
    }
    if (eh->e_machine != EM_RISCV) {
        throw std::runtime_error("Unexpected ELF machine type");
    }

    if (eh->e_phoff == 0 || eh->e_phnum == 0) {
        throw std::runtime_error("No program headers found");
    }
    if (eh->e_phentsize != sizeof(Elf32_Phdr)) {
        throw std::runtime_error("Unexpected program header size");
    }

    const auto phdr_span = view.subspan(
        static_cast<std::size_t>(eh->e_phoff),
        static_cast<std::size_t>(eh->e_phnum) * eh->e_phentsize
    );
    const auto* phdrs = reinterpret_cast<const Elf32_Phdr*>(phdr_span.data());

    for (uint16_t i = 0; i < eh->e_phnum; ++i) {
        const Elf32_Phdr& ph = phdrs[i];
        if (ph.p_type != PT_LOAD || ph.p_memsz == 0) {
            continue;
        }

        const std::size_t file_off = static_cast<std::size_t>(ph.p_offset);
        const std::size_t file_sz  = static_cast<std::size_t>(ph.p_filesz);

        if (file_off + file_sz > view.size()) {
            throw std::runtime_error("Segment exceeds file size");
        }
        const uint64_t mem_end = static_cast<uint64_t>(ph.p_vaddr) + ph.p_memsz;
        if (mem_end > memory->Size()) {
            throw std::runtime_error("Segment does not fit into memory");
        }

        memory->LoadBytes(static_cast<uint32_t>(ph.p_vaddr), view.subspan(file_off, file_sz));
        // Remaining p_memsz - p_filesz stays zeroed in the pre-cleared memory buffer.
    }

    if (entry_point) {
        *entry_point = eh->e_entry;
    }
}
