// I-type SYSTEM (ecall, ebreak)
#pragma once

#include <cassert>
#include <cstdint>
#include <memory>

#include "rvi_decode_info.hpp"
#include "rvi_instruction_interface.hpp"
#include "rvi_instruction_registry.hpp"

namespace rvi {
namespace rv32i {

class Ecall final : public IInstruction {
public:
    static constexpr uint32_t kOpcode = 0x73u;

    ExecutionStatus Execute(InterpreterState* /*state*/) override {
        assert(0);
        return ExecutionStatus::Success;
    }

    const char* GetName()   const override { return "Ecall"; }
    uint32_t    GetOpcode() const override { return kOpcode; }

    InstructionDecodedCommonType GetDecodedInfo() const override {
        InstructionDecodedInfoTypeI info = {
            .opcode = kOpcode,
            .funct3 = 0b000u,
            .imm    = 0,
        };
        return info;
    }
};

class Ebreak final : public IInstruction {
public:
    static constexpr uint32_t kOpcode = 0x73u;

    ExecutionStatus Execute(InterpreterState* /*state*/) override {
        assert(0);
        return ExecutionStatus::Success;
    }

    const char* GetName()   const override { return "Ebreak"; }
    uint32_t    GetOpcode() const override { return kOpcode; }

    InstructionDecodedCommonType GetDecodedInfo() const override {
        InstructionDecodedInfoTypeI info = {
            .opcode = kOpcode,
            .funct3 = 0b000u,
            .imm    = 1,
        };
        return info;
    }
};

namespace {

inline void RegisterInstructionsTypeI_System(rvi::InstructionRegistry* registry) {
    registry->RegisterInstruction(std::make_unique<Ebreak>());
    registry->RegisterInstruction(std::make_unique<Ecall> ());
}

inline uint32_t KeyTypeI_System(InstructionDecodedCommonType info) {
    auto i = std::get<InstructionDecodedInfoTypeI>(info);
    assert(i.imm == 0 || i.imm == 1);

    return i.imm;
}

} // namespace

inline void RegisterOpcodeGroupTypeI_System(rvi::InstructionRegistry* registry) {
    registry->RegisterGroup(rvi::PerOpcodeGroup(/*size*/ 2u, &KeyTypeI_System), Ecall::kOpcode);

    RegisterInstructionsTypeI_System(registry);
}

} // namespace rv32i
} // namespace rvi
