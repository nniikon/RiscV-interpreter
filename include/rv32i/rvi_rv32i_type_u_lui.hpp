// U-type LUI
#pragma once

#include <cstdint>
#include <memory>

#include "rvi_decode_info.hpp"
#include "rvi_instruction_interface.hpp"
#include "rvi_instruction_registry.hpp"

namespace rvi {
namespace rv32i {

class Lui : public IInstruction {
public:
    static constexpr uint32_t kOpcode = 0x37u;

    ExecutionStatus Execute(InterpreterState* state,
                            const InstructionDecodedCommonType& decoded_info) const override {
        const auto& info = std::get<InstructionDecodedInfoTypeU>(decoded_info);
        state->regs.Set(info.rd, static_cast<uint32_t>(info.imm));

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

namespace {

inline void RegisterInstructionsTypeU_Lui(rvi::InstructionRegistry* registry) {
    registry->RegisterInstruction(std::make_unique<Lui>());
}

inline uint32_t KeyTypeU_Lui(InstructionDecodedCommonType /*info*/) {
    return 0u;
}

} // namespace

inline void RegisterOpcodeGroupTypeU_Lui(rvi::InstructionRegistry* registry) {
    registry->RegisterGroup(
        rvi::PerOpcodeGroup(/*size*/ 1u, &KeyTypeU_Lui, &DecodeInstructionToCommonTypeU),
        Lui::kOpcode);

    RegisterInstructionsTypeU_Lui(registry);
}

} // namespace rv32i
} // namespace rvi
