#pragma once

#include <cstdint>
#include <span>
#include <string>

#include "rvi_mmap_file.hpp"

namespace rvi {

class ReadBinary {
    std::string path_;
    MMapRO mmap_;
public:
    ReadBinary(std::string_view path);

    std::span<const uint8_t> GetTextSectionView() const;
};

} // namespace