#pragma once

#include "rvi_instruction_interface.hpp"
#include "rvi_instruction_registry.hpp"
#include <cassert>

namespace rvi {
namespace rv32i {

template <class Oper>
class Save : public rvi::IInstructionTypeS {
    ExecutionStatus Execute(InterpreterState* state) override {
        uint32_t addr = state->regs[info_.rs1] + static_cast<int32_t>(info_.imm);

        typename Oper::type value = static_cast<typename Oper::type>(state->regs[info_.rs2]);
        state->memory.Set<typename Oper::type>(addr, value);

        state->pc += 4u;
        return ExecutionStatus::Success;
    }

    const char* GetName()           const override { return Oper::name; }
    uint32_t    GetExtendedOpcode() const override { return Oper::extended_opcode; }
};

struct SaveWord {
    using type = uint32_t;
    static constexpr const char* name = "sw";
    static constexpr uint32_t extended_opcode = 0x123; // pack_ext(0, 0b010, 0x23)
};

struct SaveHalf {
    using type = uint16_t;
    static constexpr const char* name = "sh";
    static constexpr uint32_t extended_opcode = 0x0A3; // pack_ext(0, 0b001, 0x23)
};

struct SaveByte {
    using type = uint8_t;
    static constexpr const char* name = "sb";
    static constexpr uint32_t extended_opcode = 0x023; // pack_ext(0, 0b000, 0x23)
};

using Sw = Save<SaveWord>;
using Sh = Save<SaveHalf>;
using Sb = Save<SaveByte>;

void RegisterInstructionsTypeS(rvi::InstructionRegistry* registry) {
    registry->RegisterInstruction(std::make_unique<Sw>());
    registry->RegisterInstruction(std::make_unique<Sh>());
    registry->RegisterInstruction(std::make_unique<Sb>());
}

} // namespace rv32i
} // namespace rvi
