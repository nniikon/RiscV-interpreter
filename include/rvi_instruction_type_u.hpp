#pragma once

#include "rvi_decode_info.hpp"
#include "rvi_instruction_interface.hpp"
#include "rvi_instruction_registry.hpp"

namespace rvi {
namespace rv32i {

class Lui : public IInstruction {
    static constexpr uint32_t kOpcode = 0x37u;
public:
    ExecutionStatus Execute(InterpreterState* state) override {
        auto info = std::get<InstructionDecodedInfoTypeU>(info_);
        state->regs[info.rd] = static_cast<uint32_t>(info.imm);

        return ExecutionStatus::Success;
    }

    const char* GetName()   const override { return "lui"; }
    uint32_t    GetOpcode() const override { return kOpcode; }

    InstructionDecodedCommonType GetDecodedInfo() const override {
        InstructionDecodedInfoTypeU info = {
            .opcode = kOpcode,
        };
        return info;
    }
};

class Auipc : public IInstruction {
    static constexpr uint32_t kOpcode = 0x17u;
public:
    ExecutionStatus Execute(InterpreterState* state) override {
        auto info = std::get<InstructionDecodedInfoTypeU>(info_);
        state->regs[info.rd] = state->pc + static_cast<uint32_t>(info.imm);

        return ExecutionStatus::Success;
    }

    const char* GetName()   const override { return "auipc"; }
    uint32_t    GetOpcode() const override { return kOpcode; }

    InstructionDecodedCommonType GetDecodedInfo() const override {
        InstructionDecodedInfoTypeU info = {
            .opcode = kOpcode,
        };
        return info;
    }
};

void RegisterInstructionsTypeU(rvi::InstructionRegistry* registry) {
    registry->RegisterInstruction(std::make_unique<Lui>  ());
    registry->RegisterInstruction(std::make_unique<Auipc>());
}

} // namespace rv32i
} // namespace
