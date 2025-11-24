// J-type JAL
#pragma once

#include <cstdint>
#include <memory>

#include "rvi_decode_info.hpp"
#include "rvi_instruction_interface.hpp"
#include "rvi_instruction_registry.hpp"

namespace rvi {
namespace rv32i {

class Jal final : public IInstruction {
public:
    static constexpr uint32_t kOpcode = 0x6Fu;

    ExecutionStatus Execute(InterpreterState* state,
                            const InstructionDecodedCommonType& decoded_info) const override {
        const auto& info = std::get<InstructionDecodedInfoTypeJ>(decoded_info);

        uint32_t return_addr = state->pc + 4u;
        state->regs[info.rd] = return_addr;

        state->pc = static_cast<uint32_t>(
            static_cast<int32_t>(state->pc) + info.imm
        );

        return ExecutionStatus::Success;
    }

    const char* GetName()   const override { return "jal"; }
    uint32_t    GetOpcode() const override { return kOpcode; }

    InstructionDecodedCommonType GetDecodedInfo() const override {
        InstructionDecodedInfoTypeJ info = {
            .opcode = kOpcode,
        };
        return info;
    }
};

namespace {

inline void RegisterInstructionsTypeJ_Jal(rvi::InstructionRegistry* registry) {
    registry->RegisterInstruction(std::make_unique<Jal>());
}

inline uint32_t KeyTypeJ_Jal(InstructionDecodedCommonType /*info*/) {
    return 0u;
}

} // namespace

inline void RegisterOpcodeGroupTypeJ_Jal(rvi::InstructionRegistry* registry) {
    registry->RegisterGroup(
        rvi::PerOpcodeGroup(/*size*/ 1u, &KeyTypeJ_Jal, &DecodeInstructionToCommonTypeJ),
        Jal::kOpcode);

    RegisterInstructionsTypeJ_Jal(registry);
}

} // namespace rv32i
} // namespace rvi
