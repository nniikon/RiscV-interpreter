#pragma once

#include <cstdint>

namespace rvi {
namespace rv32zbb {

constexpr uint32_t kXLen = 32u;
constexpr uint32_t kRotateMask = 0x1Fu;

inline uint32_t CountLeadingZeros(uint32_t value) {
    if (value == 0u) {
        return kXLen;
    }

    uint32_t count = 0u;
    while ((value & 0x8000'0000u) == 0u) {
        value <<= 1u;
        ++count;
    }
    return count;
}

inline uint32_t CountTrailingZeros(uint32_t value) {
    if (value == 0u) {
        return kXLen;
    }

    uint32_t count = 0u;
    while ((value & 1u) == 0u) {
        value >>= 1u;
        ++count;
    }
    return count;
}

inline uint32_t PopulationCount32(uint32_t value) {
    uint32_t count = 0u;
    while (value != 0u) {
        value &= (value - 1u);
        ++count;
    }
    return count;
}

inline uint32_t SignExtend8(uint32_t value) {
    const auto extended = static_cast<int32_t>(static_cast<int8_t>(value & 0xFFu));
    return static_cast<uint32_t>(extended);
}

inline uint32_t SignExtend16(uint32_t value) {
    const auto extended = static_cast<int32_t>(static_cast<int16_t>(value & 0xFFFFu));
    return static_cast<uint32_t>(extended);
}

inline uint32_t RotateRight32(uint32_t value, uint32_t amount) {
    const uint32_t shamt = amount & kRotateMask;
    if (shamt == 0u) {
        return value;
    }
    return (value >> shamt) | (value << (kXLen - shamt));
}

inline uint32_t RotateLeft32(uint32_t value, uint32_t amount) {
    const uint32_t sh = amount & kRotateMask;
    if (sh == 0u) {
        return value;
    }
    return (value << sh) | (value >> (kXLen - sh));
}

inline uint32_t OrcByte(uint32_t value) {
    uint32_t result = 0u;
    for (uint32_t shift = 0u; shift < kXLen; shift += 8u) {
        const uint32_t byte = (value >> shift) & 0xFFu;
        if (byte != 0u) {
            result |= (0xFFu << shift);
        }
    }
    return result;
}

inline uint32_t ReverseBytes(uint32_t value) {
    return ((value & 0x0000'00FFu) << 24) |
           ((value & 0x0000'FF00u) << 8)  |
           ((value & 0x00FF'0000u) >> 8)  |
           ((value & 0xFF00'0000u) >> 24);
}

} // rv32bzz

} // rvi