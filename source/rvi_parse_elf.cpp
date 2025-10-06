#include "rvi_parse_elf.hpp"

#include <cstdint>
#include <cstring>
#include <elf.h>
#include <fstream>
#include <vector>

bool rvi::elf::GetTextSectionOffsetSize(std::string_view path, uint32_t& out_offset, uint32_t& out_size) {
    std::ifstream f(path.data(), std::ios::binary);
    if (!f) return false;

    Elf32_Ehdr eh{};
    if (!f.read(reinterpret_cast<char*>(&eh), sizeof(eh))) return false;

    if (std::memcmp(eh.e_ident, ELFMAG, SELFMAG) != 0) return false;
    if (eh.e_ident[EI_CLASS] != ELFCLASS32)            return false;
    if (eh.e_ident[EI_DATA]  != ELFDATA2LSB)           return false;
    if (eh.e_machine != EM_RISCV)                      return false;

    if (eh.e_shoff == 0 || eh.e_shnum == 0)            return false;
    if (eh.e_shentsize != sizeof(Elf32_Shdr))          return false;

    f.seekg(static_cast<std::streamoff>(eh.e_shoff), std::ios::beg);

    std::vector<Elf32_Shdr> section_headers(eh.e_shnum);
    if (!f.read(reinterpret_cast<char*>(section_headers.data()),
                static_cast<std::streamsize>(section_headers.size() * sizeof(Elf32_Shdr)))) {
        return false;
    }

    if (eh.e_shstrndx == SHN_UNDEF || eh.e_shstrndx >= eh.e_shnum) return false;
    const Elf32_Shdr& section_string = section_headers[eh.e_shstrndx];

    std::vector<char> shstrtab(section_string.sh_size);
    f.seekg(static_cast<std::streamoff>(section_string.sh_offset), std::ios::beg);
    if (!f.read(shstrtab.data(), static_cast<std::streamsize>(shstrtab.size()))) {
        return false;
    }

    for (const auto& sh : section_headers) {
        if (sh.sh_name >= shstrtab.size())
            continue;

        const char* name = &shstrtab[sh.sh_name];
        if (std::strcmp(name, ".text") == 0) {
            out_offset = sh.sh_offset;
            out_size   = sh.sh_size;
            return true;
        }
    }
    return false;
}