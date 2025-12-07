#pragma once

#include "rvi_config.hpp"

#include <array>
#include <cassert>
#include <cstdint>

namespace rvi {

class InterpreterRegisters {
public:
    InterpreterRegisters() = default;

    uint32_t Get(uint32_t index) const noexcept {
        assert(index < static_cast<uint32_t>(kNumRegs));
        if (index == 0u) {
            return 0u;
        }
        return regs_[index];
    }

    void Set(uint32_t index, uint32_t value) {
        assert(index < static_cast<uint32_t>(kNumRegs));
        if (index == 0) {
            return;
        }
        regs_[index] = value;
    }

private:
    std::array<uint32_t, kNumRegs> regs_{};
};

class InterpreterRegistersFloat {
public:
    InterpreterRegistersFloat() = default;

    float Get(uint32_t index) const noexcept {
        assert(index < static_cast<uint32_t>(kNumRegs));

        return regs_[index];
    }

    void Set(uint32_t index, float value) {
        assert(index < static_cast<uint32_t>(kNumRegs));

        regs_[index] = value;
    }

private:
    std::array<float, kNumRegs> regs_{};
};

} // namespace rvi
