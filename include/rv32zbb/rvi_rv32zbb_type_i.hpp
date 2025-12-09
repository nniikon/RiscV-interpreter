#pragma once

#include <cstdint>
#include <memory>

#include "rvi_decode_info.hpp"
#include "rvi_instruction_interface.hpp"
#include "rvi_instruction_registry.hpp"

#include "rvi_rv32zbb_common_helpers.hpp"

namespace rvi {
namespace rv32zbb {

template <class Oper>
class InstructionTypeI final : public IInstruction {
public:
    static constexpr uint32_t kOpcode = 0x13u;

    ExecutionStatus Execute(InterpreterState* state,
                            const InstructionDecodedCommonType& decoded_info) const override {
        const auto& info = std::get<InstructionDecodedInfoTypeI>(decoded_info);
        Oper::exec(state, info);
        state->pc += 4u;
        return ExecutionStatus::Success;
    }

    const char* GetName()   const override { return Oper::name; }
    uint32_t    GetOpcode() const override { return kOpcode; }

    InstructionDecodedCommonType GetDecodedInfo() const override {
        InstructionDecodedInfoTypeI info = {
            .opcode = kOpcode,
            .funct3 = Oper::funct3,
            .imm    = static_cast<int32_t>(Oper::imm),
        };
        return info;
    }
};

struct ClzOper {
    constexpr static const char* const name = "clz";
    static constexpr uint32_t funct3 = 0b001u;
    static constexpr uint32_t imm = 0b011000000000u;

    static void exec(InterpreterState* state, const InstructionDecodedInfoTypeI& info) {
        const uint32_t value = state->regs.Get(info.rs1);
        const uint32_t result = CountLeadingZeros(value);
        state->regs.Set(info.rd, result);
    }
};

struct CtzOper {
    constexpr static const char* const name = "ctz";
    static constexpr uint32_t funct3 = 0b001u;
    static constexpr uint32_t imm = 0b011000000001u;

    static void exec(InterpreterState* state, const InstructionDecodedInfoTypeI& info) {
        const uint32_t value = state->regs.Get(info.rs1);
        const uint32_t result = CountTrailingZeros(value);
        state->regs.Set(info.rd, result);
    }
};

struct CpopOper {
    constexpr static const char* const name = "cpop";
    static constexpr uint32_t funct3 = 0b001u;
    static constexpr uint32_t imm = 0b011000000010u;

    static void exec(InterpreterState* state, const InstructionDecodedInfoTypeI& info) {
        const uint32_t value = state->regs.Get(info.rs1);
        const uint32_t result = PopulationCount32(value);
        state->regs.Set(info.rd, result);
    }
};

struct SextBOper {
    constexpr static const char* const name = "sext.b";
    static constexpr uint32_t funct3 = 0b001u;
    static constexpr uint32_t imm = 0b011000000100u;

    static void exec(InterpreterState* state, const InstructionDecodedInfoTypeI& info) {
        const uint32_t value = state->regs.Get(info.rs1);
        state->regs.Set(info.rd, SignExtend8(value));
    }
};

struct SextHOper {
    constexpr static const char* const name = "sext.h";
    static constexpr uint32_t funct3 = 0b001u;
    static constexpr uint32_t imm = 0b011000000101u;

    static void exec(InterpreterState* state, const InstructionDecodedInfoTypeI& info) {
        const uint32_t value = state->regs.Get(info.rs1);
        state->regs.Set(info.rd, SignExtend16(value));
    }
};

struct RoriOper {
    constexpr static const char* const name = "rori";
    static constexpr uint32_t funct3 = 0b101u;
    static constexpr uint32_t imm = 0b011000000000u;

    static void exec(InterpreterState* state, const InstructionDecodedInfoTypeI& info) {
        const uint32_t value = state->regs.Get(info.rs1);
        const uint32_t shamt = static_cast<uint32_t>(info.imm) & kRotateMask;
        state->regs.Set(info.rd, RotateRight32(value, shamt));
    }
};

struct OrcbOper {
    constexpr static const char* const name = "orc.b";
    static constexpr uint32_t funct3 = 0b101u;
    static constexpr uint32_t imm = 0b001010000111u;

    static void exec(InterpreterState* state, const InstructionDecodedInfoTypeI& info) {
        const uint32_t value = state->regs.Get(info.rs1);
        state->regs.Set(info.rd, OrcByte(value));
    }
};

struct Rev8Oper {
    constexpr static const char* const name = "rev8";
    static constexpr uint32_t funct3 = 0b101u;
    static constexpr uint32_t imm = 0b011010011000u;

    static void exec(InterpreterState* state, const InstructionDecodedInfoTypeI& info) {
        const uint32_t value = state->regs.Get(info.rs1);
        state->regs.Set(info.rd, ReverseBytes(value));
    }
};

using Clz   = InstructionTypeI<ClzOper>;
using Ctz   = InstructionTypeI<CtzOper>;
using Cpop  = InstructionTypeI<CpopOper>;
using SextB = InstructionTypeI<SextBOper>;
using SextH = InstructionTypeI<SextHOper>;
using Rori  = InstructionTypeI<RoriOper>;
using Orcb  = InstructionTypeI<OrcbOper>;
using Rev8  = InstructionTypeI<Rev8Oper>;

inline void RegisterInstructionsTypeI(rvi::InstructionRegistry* registry) {
    registry->RegisterInstruction(std::make_unique<Clz>());
    registry->RegisterInstruction(std::make_unique<Ctz>());
    registry->RegisterInstruction(std::make_unique<Cpop>());
    registry->RegisterInstruction(std::make_unique<SextB>());
    registry->RegisterInstruction(std::make_unique<SextH>());
    registry->RegisterInstruction(std::make_unique<Rori>());
    registry->RegisterInstruction(std::make_unique<Orcb>());
    registry->RegisterInstruction(std::make_unique<Rev8>());
}

} // namespace rv32zbb
} // namespace rvi

