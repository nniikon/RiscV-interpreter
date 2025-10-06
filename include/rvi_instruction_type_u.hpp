#pragma once

#include "rvi_instruction_interface.hpp"
#include "rvi_instruction_registry.hpp"

namespace rvi {

class Lui : public IInstructionTypeU {

    ExecutionStatus Execute(InterpreterState* state) override {
        state->regs[info_.rd] = info_.opcode << 12;

        return ExecutionStatus::Success;
    }

    const char* GetName()           const override { return "lui"; }
    uint32_t    GetExtendedOpcode() const override { return 0x00; }
};

class Auipc : public IInstructionTypeU {

    ExecutionStatus Execute(InterpreterState* state) override {
        state->regs[info_.rd] = state->pc + (info_.opcode << 12);

        return ExecutionStatus::Success;
    }

    const char* GetName()           const override { return "auipc"; }
    uint32_t    GetExtendedOpcode() const override { return 0x00; }
};

void RegisterInstructionsTypeU(rvi::InstructionRegistry* registry) {
    registry->RegisterInstruction(std::make_unique<Lui>  ());
    registry->RegisterInstruction(std::make_unique<Auipc>());
}

} // namespace