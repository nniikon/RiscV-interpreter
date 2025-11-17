// I-type JALR
#pragma once

#include <cstdint>
#include <memory>

#include "rvi_decode_info.hpp"
#include "rvi_instruction_interface.hpp"
#include "rvi_instruction_registry.hpp"

namespace rvi {
namespace rv32i {

class Jalr final : public IInstruction {
public:
    static constexpr uint32_t kOpcode = 0x67u;

    ExecutionStatus Execute(InterpreterState* state) const override {
        auto info = std::get<InstructionDecodedInfoTypeI>(info_);

        uint32_t addr = static_cast<uint32_t>(
            static_cast<int32_t>(state->regs[info.rs1]) + info.imm
        ) & ~1;

        uint32_t return_addr = state->pc + 4u;
        state->regs[info.rd] = return_addr;

        state->pc = addr;

        return ExecutionStatus::Success;
    }

    const char* GetName()   const override { return "jalr"; };
    uint32_t    GetOpcode() const override { return kOpcode; }

    InstructionDecodedCommonType GetDecodedInfo() const override {
        InstructionDecodedInfoTypeI info = {
            .opcode = kOpcode,
            .funct3 = 0b000,
        };

        return info;
    }
};

namespace {

inline void RegisterInstructionsTypeI_Jalr(rvi::InstructionRegistry* registry) {
    registry->RegisterInstruction(std::make_unique<Jalr>());
}

inline uint32_t KeyTypeI_Jalr(InstructionDecodedCommonType info) {
    return 0;
}

} // namespace

inline void RegisterOpcodeGroupTypeI_Jalr(rvi::InstructionRegistry* registry) {
    registry->RegisterGroup(
        rvi::PerOpcodeGroup(/*size*/ 8u, &KeyTypeI_Jalr, &DecodeInstructionToCommonTypeI),
        Jalr::kOpcode);

    RegisterInstructionsTypeI_Jalr(registry);
}

} // namespace rv32i
} // namespace rvi
