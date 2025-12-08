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
class InstructionTypeR final : public IInstruction {
public:
    static constexpr uint32_t kOpcode = 0x33u;

    ExecutionStatus Execute(InterpreterState* state,
                            const InstructionDecodedCommonType& decoded_info) const override {
        const auto& info = std::get<InstructionDecodedInfoTypeR>(decoded_info);
        const uint32_t lhs = state->regs.Get(info.rs1);
        const uint32_t rhs = state->regs.Get(info.rs2);

        state->regs.Set(info.rd, Oper::exec(lhs, rhs));
        state->pc += 4u;

        return ExecutionStatus::Success;
    }

    const char* GetName()   const override { return Oper::name; }
    uint32_t    GetOpcode() const override { return kOpcode; }

    InstructionDecodedCommonType GetDecodedInfo() const override {
        InstructionDecodedInfoTypeR info = {
            .opcode = kOpcode,
            .funct3 = Oper::funct3,
            .funct7 = Oper::funct7,
        };
        return info;
    }
};

struct AndnOp {
    constexpr static const char* const name = "andn";

    static uint32_t exec(uint32_t lhs, uint32_t rhs) {
        return lhs & ~rhs;
    }

    static constexpr uint32_t funct3 = 0b111u;
    static constexpr uint32_t funct7 = 0b0100000u;
};

struct OrnOp {
    constexpr static const char* const name = "orn";

    static uint32_t exec(uint32_t lhs, uint32_t rhs) {
        return lhs | ~rhs;
    }

    static constexpr uint32_t funct3 = 0b110u;
    static constexpr uint32_t funct7 = 0b0100000u;
};

struct XnorOp {
    constexpr static const char* const name = "xnor";

    static uint32_t exec(uint32_t lhs, uint32_t rhs) {
        return ~(lhs ^ rhs);
    }

    static constexpr uint32_t funct3 = 0b100u;
    static constexpr uint32_t funct7 = 0b0100000u;
};

struct MinOp {
    constexpr static const char* const name = "min";

    static uint32_t exec(uint32_t lhs, uint32_t rhs) {
        const int32_t lhs_signed = static_cast<int32_t>(lhs);
        const int32_t rhs_signed = static_cast<int32_t>(rhs);
        return (lhs_signed < rhs_signed) ? lhs : rhs;
    }

    static constexpr uint32_t funct3 = 0b100u;
    static constexpr uint32_t funct7 = 0b0000101u;
};

struct MaxOp {
    constexpr static const char* const name = "max";

    static uint32_t exec(uint32_t lhs, uint32_t rhs) {
        const int32_t lhs_signed = static_cast<int32_t>(lhs);
        const int32_t rhs_signed = static_cast<int32_t>(rhs);
        return (lhs_signed >= rhs_signed) ? lhs : rhs;
    }

    static constexpr uint32_t funct3 = 0b110u;
    static constexpr uint32_t funct7 = 0b0000101u;
};

struct MinuOp {
    constexpr static const char* const name = "minu";

    static uint32_t exec(uint32_t lhs, uint32_t rhs) {
        return (lhs < rhs) ? lhs : rhs;
    }

    static constexpr uint32_t funct3 = 0b101u;
    static constexpr uint32_t funct7 = 0b0000101u;
};

struct MaxuOp {
    constexpr static const char* const name = "maxu";

    static uint32_t exec(uint32_t lhs, uint32_t rhs) {
        return (lhs >= rhs) ? lhs : rhs;
    }

    static constexpr uint32_t funct3 = 0b111u;
    static constexpr uint32_t funct7 = 0b0000101u;
};

struct RolOp {
    constexpr static const char* const name = "rol";

    static uint32_t exec(uint32_t lhs, uint32_t rhs) {
        return RotateLeft32(lhs, rhs);
    }

    static constexpr uint32_t funct3 = 0b001u;
    static constexpr uint32_t funct7 = 0b0110000u;
};

struct RorOp {
    constexpr static const char* const name = "ror";

    static uint32_t exec(uint32_t lhs, uint32_t rhs) {
        return RotateRight32(lhs, rhs);
    }

    static constexpr uint32_t funct3 = 0b101u;
    static constexpr uint32_t funct7 = 0b0110000u;
};

struct ZextOp {
    constexpr static const char* const name = "zext.h";

    static uint32_t exec(uint32_t lhs, uint32_t /*rhs*/) {
        return lhs & 0x0000FFFFu;
    }

    static constexpr uint32_t funct3 = 0b100u;
    static constexpr uint32_t funct7 = 0b0000100u;
};

using Andn  = InstructionTypeR<AndnOp>;
using Orn   = InstructionTypeR<OrnOp>;
using Xnor  = InstructionTypeR<XnorOp>;
using Min   = InstructionTypeR<MinOp>;
using Max   = InstructionTypeR<MaxOp>;
using Minu  = InstructionTypeR<MinuOp>;
using Maxu  = InstructionTypeR<MaxuOp>;
using Rol   = InstructionTypeR<RolOp>;
using Ror   = InstructionTypeR<RorOp>;
using Zext  = InstructionTypeR<ZextOp>;

inline void RegisterInstructionsTypeR(rvi::InstructionRegistry* registry) {
    registry->RegisterInstruction(std::make_unique<Andn>());
    registry->RegisterInstruction(std::make_unique<Orn>());
    registry->RegisterInstruction(std::make_unique<Xnor>());
    registry->RegisterInstruction(std::make_unique<Min>());
    registry->RegisterInstruction(std::make_unique<Max>());
    registry->RegisterInstruction(std::make_unique<Minu>());
    registry->RegisterInstruction(std::make_unique<Maxu>());
    registry->RegisterInstruction(std::make_unique<Rol>());
    registry->RegisterInstruction(std::make_unique<Ror>());
    registry->RegisterInstruction(std::make_unique<Zext>());
}

} // namespace rv32zbb
} // namespace rvi
