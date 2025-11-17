// I-type FENCE
#pragma once

#include <cassert>
#include <cstdint>
#include <memory>

#include "rvi_decode_info.hpp"
#include "rvi_instruction_interface.hpp"
#include "rvi_instruction_registry.hpp"

namespace rvi {
namespace rv32i {

class Fence final : public IInstruction {
public:
    static constexpr uint32_t kOpcode = 0x0Fu;

    ExecutionStatus Execute(InterpreterState* /*state*/) const override {
        assert(0);
        return ExecutionStatus::Success;
    }

    const char* GetName()   const override { return "Fence"; }
    uint32_t    GetOpcode() const override { return kOpcode; }

    InstructionDecodedCommonType GetDecodedInfo() const override {
        InstructionDecodedInfoTypeI info = {
            .opcode = kOpcode,
            .funct3 = 0b000u,
        };
        return info;
    }
};

namespace {

inline void RegisterInstructionsTypeI_Fence(rvi::InstructionRegistry* registry) {
    registry->RegisterInstruction(std::make_unique<Fence>());
}

inline uint32_t KeyTypeI_Fence(InstructionDecodedCommonType info) {
    return 0;
}

} // namespace

inline void RegisterOpcodeGroupTypeI_Fence(rvi::InstructionRegistry* registry) {
    registry->RegisterGroup(
        rvi::PerOpcodeGroup(/*size*/ 1u, &KeyTypeI_Fence, &DecodeInstructionToCommonTypeI),
        Fence::kOpcode);
}

} // namespace rv32i
} // namespace rvi
