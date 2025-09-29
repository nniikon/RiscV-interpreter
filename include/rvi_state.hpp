#pragma once

#include "rvi_config.hpp"
#include "rvi_memory_state.hpp"
#include <array>
#include <cstdint>

namespace rvi {

struct InterpreterState {
    std::array<uint32_t, kNumRegs> regs;
    uint32_t pc;
    InterpreterMemoryModel memory;
};

} // namespace