// Helper for the SLT and SLTU R-type comparisons
#pragma once

#include <cstdint>
#include <memory>

#include "rv32i/rvi_rv32i_type_r.hpp"
#include "rvi_instruction_registry.hpp"

namespace rvi {
namespace rv32i {

struct SltCompareOp {
    constexpr static const char* const name = "slt";

    static uint32_t exec(uint32_t a, uint32_t b) {
        return static_cast<int32_t>(a) < static_cast<int32_t>(b) ? 1u : 0u;
    }

    static constexpr uint32_t funct3 = 0b010u;
    static constexpr uint32_t funct7 = 0x00u;
};

struct SltuCompareOp {
    constexpr static const char* const name = "sltu";

    static uint32_t exec(uint32_t a, uint32_t b) {
        return a < b ? 1u : 0u;
    }

    static constexpr uint32_t funct3 = 0b011u;
    static constexpr uint32_t funct7 = 0x00u;
};

using Slt  = InstructionTypeR<SltCompareOp>;
using Sltu = InstructionTypeR<SltuCompareOp>;

inline void RegisterInstructionsTypeR_Compare(rvi::InstructionRegistry* registry) {
    registry->RegisterInstruction(std::make_unique<Slt>());
    registry->RegisterInstruction(std::make_unique<Sltu>());
}

} // namespace rv32i
} // namespace rvi
