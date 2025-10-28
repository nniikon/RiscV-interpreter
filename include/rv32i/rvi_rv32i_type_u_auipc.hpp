// U-type AUIPC
#pragma once

#include <cstdint>
#include <memory>

#include "rvi_decode_info.hpp"
#include "rvi_instruction_interface.hpp"
#include "rvi_instruction_registry.hpp"

namespace rvi {
namespace rv32i {

class Auipc final : public IInstruction {
public:
    static constexpr uint32_t kOpcode = 0x17u;

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

namespace {

inline void RegisterInstructionsTypeU_Auipc(rvi::InstructionRegistry* registry) {
    registry->RegisterInstruction(std::make_unique<Auipc>());
}

inline uint32_t KeyTypeU_Auipc(InstructionDecodedCommonType /*info*/) {
    return 0u;
}

} // namespace

inline void RegisterOpcodeGroupTypeU_Auipc(rvi::InstructionRegistry* registry) {
    registry->RegisterGroup(rvi::PerOpcodeGroup(/*size*/ 1u, &KeyTypeU_Auipc), Auipc::kOpcode);

    RegisterInstructionsTypeU_Auipc(registry);
}

} // namespace rv32i
} // namespace rvi
