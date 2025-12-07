// Implements fsw

#pragma once

#include <cstdint>
#include <memory>

#include "rvi_decode_info.hpp"
#include "rvi_instruction_interface.hpp"
#include "rvi_instruction_registry.hpp"

namespace rvi {
namespace rv32f {

class Fsw final : public IInstruction {
public:
    static constexpr uint32_t kOpcode = 0x27u;

    ExecutionStatus Execute(InterpreterState* state,
                            const InstructionDecodedCommonType& decoded_info) const override {
        const auto& info = std::get<InstructionDecodedInfoTypeS>(decoded_info);

        uint32_t addr = static_cast<uint32_t>(
            static_cast<int32_t>(state->regs.Get(info.rs1)) + info.imm
        );

        float value = state->f_regs.Get(info.rs2);
        state->memory.Set<float>(addr, value);

        state->pc += 4u;
        return ExecutionStatus::Success;
    }

    const char* GetName()   const override { return "fsw"; }
    uint32_t    GetOpcode() const override { return kOpcode; }

    InstructionDecodedCommonType GetDecodedInfo() const override {
        InstructionDecodedInfoTypeS info = {
            .opcode = kOpcode,
            .funct3 = 0b010u,
        };
        return info;
    }
};

namespace {

inline void RegisterInstructionsTypeS_Fsw(rvi::InstructionRegistry* registry) {
    registry->RegisterInstruction(std::make_unique<Fsw>());
}

inline uint32_t KeyTypeS_Fsw(InstructionDecodedCommonType /*info*/) {
    return 0u;
}

} // namespace

inline void RegisterOpcodeGroupTypeS_Fsw(rvi::InstructionRegistry* registry) {
    registry->RegisterGroup(
        rvi::PerOpcodeGroup(/*size*/ 0u, &KeyTypeS_Fsw, &DecodeInstructionToCommonTypeS),
        Fsw::kOpcode
    );

    RegisterInstructionsTypeS_Fsw(registry);
}

} // namespace rv32f
} // namespace rvi
