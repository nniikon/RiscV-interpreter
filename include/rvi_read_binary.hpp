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
    struct SectionInfo {
        std::span<const uint8_t> section;
        uint32_t start_offset;
    };

    ReadBinary(std::string_view path);

    SectionInfo GetTextSectionView() const;
    uint32_t                 GetStartOffset()     const;
};

} // namespace