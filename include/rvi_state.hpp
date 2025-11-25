#pragma once

#include "rvi_memory_state.hpp"
#include "rvi_registers.hpp"
#include <cstdint>

namespace rvi {

struct InterpreterState {
    InterpreterRegisters regs;
    uint32_t pc;
    InterpreterMemoryModel memory;
    int32_t return_code;
};

} // namespace
