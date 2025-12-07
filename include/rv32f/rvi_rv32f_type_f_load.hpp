// Implements flw

#pragma once

#include <cstdint>
#include <memory>

#include "rvi_decode_info.hpp"
#include "rvi_instruction_interface.hpp"
#include "rvi_instruction_registry.hpp"

namespace rvi {
namespace rv32f {

class Flw final : public IInstruction {
public:
    static constexpr uint32_t kOpcode = 0x07;

    ExecutionStatus Execute(InterpreterState* state,
                            const InstructionDecodedCommonType& decoded_info) const override {
        const auto& info = std::get<InstructionDecodedInfoTypeI>(decoded_info);

        uint32_t addr = static_cast<uint32_t>(
            static_cast<int32_t>(state->regs.Get(info.rs1)) + info.imm
        );

        auto value = state->memory.Get<float>(addr);

        state->f_regs.Set(info.rd, value);

        state->pc += 4u;

        return ExecutionStatus::Success;
    }

    const char* GetName()   const override { return "flw"; }
    uint32_t    GetOpcode() const override { return kOpcode; }

    InstructionDecodedCommonType GetDecodedInfo() const override {
        InstructionDecodedInfoTypeI info = {
            .opcode = kOpcode,
            .funct3 = 0b010,
        };
        return info;
    }
};

namespace {

inline void RegisterInstructionsTypeI_Flw(rvi::InstructionRegistry* registry) {
    registry->RegisterInstruction(std::make_unique<Flw>());
}

inline uint32_t KeyTypeI_Flw(InstructionDecodedCommonType /*info*/) {
    return 0u;
}

} // namespace

inline void RegisterOpcodeGroupTypeI_Flw(rvi::InstructionRegistry* registry) {
    registry->RegisterGroup(
        rvi::PerOpcodeGroup(/*size*/ 1u, &KeyTypeI_Flw, &DecodeInstructionToCommonTypeI), Flw::kOpcode);

    RegisterInstructionsTypeI_Flw(registry);
}

} // namespace rv32f
} // namespace rvi
