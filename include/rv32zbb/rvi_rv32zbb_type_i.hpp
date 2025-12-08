// RV32Zbb I-type operations (bit-manip immediates)
#pragma once

#include <cstdint>
#include <memory>

#include "rvi_decode_info.hpp"
#include "rvi_instruction_interface.hpp"
#include "rvi_instruction_registry.hpp"

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

namespace {

constexpr uint32_t kXLen = 32u;
constexpr uint32_t kShamtMask = kXLen - 1u;

inline uint32_t CountLeadingZeros(uint32_t value) {
    if (value == 0u) {
        return kXLen;
    }

    uint32_t count = 0u;
    while ((value & 0x8000'0000u) == 0u) {
        value <<= 1u;
        ++count;
    }
    return count;
}

inline uint32_t CountTrailingZeros(uint32_t value) {
    if (value == 0u) {
        return kXLen;
    }

    uint32_t count = 0u;
    while ((value & 1u) == 0u) {
        value >>= 1u;
        ++count;
    }
    return count;
}

inline uint32_t PopulationCount32(uint32_t value) {
    uint32_t count = 0u;
    while (value != 0u) {
        value &= (value - 1u);
        ++count;
    }
    return count;
}

inline uint32_t SignExtend8(uint32_t value) {
    const auto extended = static_cast<int32_t>(static_cast<int8_t>(value & 0xFFu));
    return static_cast<uint32_t>(extended);
}

inline uint32_t SignExtend16(uint32_t value) {
    const auto extended = static_cast<int32_t>(static_cast<int16_t>(value & 0xFFFFu));
    return static_cast<uint32_t>(extended);
}

inline uint32_t RotateRight32(uint32_t value, uint32_t amount) {
    const uint32_t shamt = amount & kShamtMask;
    if (shamt == 0u) {
        return value;
    }
    return (value >> shamt) | (value << (kXLen - shamt));
}

inline uint32_t OrcByte(uint32_t value) {
    uint32_t result = 0u;
    for (uint32_t shift = 0u; shift < kXLen; shift += 8u) {
        const uint32_t byte = (value >> shift) & 0xFFu;
        if (byte != 0u) {
            result |= (0xFFu << shift);
        }
    }
    return result;
}

inline uint32_t ReverseBytes(uint32_t value) {
    return ((value & 0x0000'00FFu) << 24) |
           ((value & 0x0000'FF00u) << 8)  |
           ((value & 0x00FF'0000u) >> 8)  |
           ((value & 0xFF00'0000u) >> 24);
}

} // namespace

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
        const uint32_t shamt = static_cast<uint32_t>(info.imm) & kShamtMask;
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

