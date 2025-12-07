#pragma once

#include <cmath>
#include <cstdint>
#include <memory>

#include "rvi_decode_info.hpp"
#include "rvi_instruction_interface.hpp"
#include "rvi_instruction_registry.hpp"

namespace rvi {
namespace rv32f {

template <class Oper>
class InstructionTypeR4 final : public IInstruction {
public:
    static constexpr uint32_t kOpcode = Oper::opcode;

    ExecutionStatus Execute(InterpreterState* state,
                            const InstructionDecodedCommonType& decoded_info) const override {
        const auto& info = std::get<InstructionDecodedInfoTypeR4>(decoded_info);

        const float lhs = state->f_regs.Get(info.rs1);
        const float rhs = state->f_regs.Get(info.rs2);
        const float acc = state->f_regs.Get(info.rs3);

        const float result = Oper::exec(lhs, rhs, acc);

        state->f_regs.Set(info.rd, result);
        state->pc += 4u;

        return ExecutionStatus::Success;
    }

    const char* GetName() const override { return Oper::name; }
    uint32_t    GetOpcode() const override { return kOpcode; }

    InstructionDecodedCommonType GetDecodedInfo() const override {
        InstructionDecodedInfoTypeR4 info = {
            .opcode = kOpcode,
            .rd = 0u,
            .rm = Oper::rm,
            .rs1 = 0u,
            .rs2 = 0u,
            .rs3 = 0u,
            .fmt = Oper::fmt,
        };

        return info;
    }
};

struct FmaddSOper {
    constexpr static const char* const name = "fmadd.s";
    constexpr static uint32_t opcode = 0x43u;
    constexpr static uint32_t rm = 0u;
    constexpr static uint32_t fmt = 0b00u;

    static float exec(float lhs, float rhs, float acc) {
        return std::fma(lhs, rhs, acc);
    }
};

struct FmsubSOper {
    constexpr static const char* const name = "fmsub.s";
    constexpr static uint32_t opcode = 0x47u;
    constexpr static uint32_t rm = 0u;
    constexpr static uint32_t fmt = 0b00u;

    static float exec(float lhs, float rhs, float acc) {
        return std::fma(lhs, rhs, -acc);
    }
};

struct FnmaddSOper {
    constexpr static const char* const name = "fnmadd.s";
    constexpr static uint32_t opcode = 0x4Fu;
    constexpr static uint32_t rm = 0u;
    constexpr static uint32_t fmt = 0b00u;

    static float exec(float lhs, float rhs, float acc) {
        return std::fma(-lhs, rhs, -acc);
    }
};

struct FnmsubSOper {
    constexpr static const char* const name = "fnmsub.s";
    constexpr static uint32_t opcode = 0x4Bu;
    constexpr static uint32_t rm = 0u;
    constexpr static uint32_t fmt = 0b00u;

    static float exec(float lhs, float rhs, float acc) {
        return std::fma(-lhs, rhs, acc);
    }
};

using FmaddS  = InstructionTypeR4<FmaddSOper>;
using FmsubS  = InstructionTypeR4<FmsubSOper>;
using FnmaddS = InstructionTypeR4<FnmaddSOper>;
using FnmsubS = InstructionTypeR4<FnmsubSOper>;

namespace {

inline void RegisterInstructionsTypeR4_Fmadd(rvi::InstructionRegistry* registry) {
    registry->RegisterInstruction(std::make_unique<FmaddS>());
}

inline void RegisterInstructionsTypeR4_Fmsub(rvi::InstructionRegistry* registry) {
    registry->RegisterInstruction(std::make_unique<FmsubS>());
}

inline void RegisterInstructionsTypeR4_Fnmadd(rvi::InstructionRegistry* registry) {
    registry->RegisterInstruction(std::make_unique<FnmaddS>());
}

inline void RegisterInstructionsTypeR4_Fnmsub(rvi::InstructionRegistry* registry) {
    registry->RegisterInstruction(std::make_unique<FnmsubS>());
}

inline uint32_t KeyTypeR4_Fma(InstructionDecodedCommonType /*info*/) {
    return 0u;
}

} // namespace

inline void RegisterOpcodeGroupTypeR4_Fmadd(rvi::InstructionRegistry* registry) {
    registry->RegisterGroup(
        rvi::PerOpcodeGroup(/*size*/ 1u, &KeyTypeR4_Fma, &DecodeInstructionToCommonTypeR4),
        FmaddS::kOpcode);

    RegisterInstructionsTypeR4_Fmadd(registry);
}

inline void RegisterOpcodeGroupTypeR4_Fmsub(rvi::InstructionRegistry* registry) {
    registry->RegisterGroup(
        rvi::PerOpcodeGroup(/*size*/ 1u, &KeyTypeR4_Fma, &DecodeInstructionToCommonTypeR4),
        FmsubS::kOpcode);

    RegisterInstructionsTypeR4_Fmsub(registry);
}

inline void RegisterOpcodeGroupTypeR4_Fnmadd(rvi::InstructionRegistry* registry) {
    registry->RegisterGroup(
        rvi::PerOpcodeGroup(/*size*/ 1u, &KeyTypeR4_Fma, &DecodeInstructionToCommonTypeR4),
        FnmaddS::kOpcode);

    RegisterInstructionsTypeR4_Fnmadd(registry);
}

inline void RegisterOpcodeGroupTypeR4_Fnmsub(rvi::InstructionRegistry* registry) {
    registry->RegisterGroup(
        rvi::PerOpcodeGroup(/*size*/ 1u, &KeyTypeR4_Fma, &DecodeInstructionToCommonTypeR4),
        FnmsubS::kOpcode);

    RegisterInstructionsTypeR4_Fnmsub(registry);
}

} // namespace rv32f
} // namespace rvi
