#include "rvi_read_binary.hpp"

#include "rvi_parse_elf.hpp"
#include <exception>
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