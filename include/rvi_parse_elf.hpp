#pragma once

#include <cstdint>
#include <elf.h>
#include <span>
#include <string_view>

namespace rvi {

namespace elf {

bool GetTextSectionOffsetSize(std::string_view path, uint32_t& out_offset, uint32_t& out_size);

std::span<const uint8_t> GetTextSectionView();

} // namespace elf

} // namespace rvi