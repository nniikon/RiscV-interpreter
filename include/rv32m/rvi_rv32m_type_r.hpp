// R-type multiply/divide with 32-bit operands (extension M)
#pragma once

#include <cstdint>
#include <limits>
#include <memory>

#include "rvi_decode_info.hpp"
#include "rvi_instruction_interface.hpp"
#include "rvi_instruction_registry.hpp"

namespace rvi {
namespace rv32m {

template <class Oper>
class InstructionTypeR final : public IInstruction {
public:
    static constexpr uint32_t kOpcode = 0x33u;

    ExecutionStatus Execute(InterpreterState* state,
                            const InstructionDecodedCommonType& decoded_info) const override {
        const auto& info = std::get<InstructionDecodedInfoTypeR>(decoded_info);
        state->regs.Set(info.rd,
                        Oper::exec(state->regs.Get(info.rs1),
                                   state->regs.Get(info.rs2)));
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

namespace {

inline uint32_t SignedMulLower(uint32_t lhs, uint32_t rhs) {
    const auto product = static_cast<int64_t>(static_cast<int32_t>(lhs)) *
                         static_cast<int64_t>(static_cast<int32_t>(rhs));
    return static_cast<uint32_t>(product);
}

inline uint32_t SignedMulHigh(uint32_t lhs, uint32_t rhs) {
    const auto product = static_cast<int64_t>(static_cast<int32_t>(lhs)) *
                         static_cast<int64_t>(static_cast<int32_t>(rhs));
    return static_cast<uint32_t>(static_cast<uint64_t>(product) >> 32);
}

inline uint32_t SignedUnsignedMulHigh(uint32_t lhs, uint32_t rhs) {
    const auto signed_lhs = static_cast<int64_t>(static_cast<int32_t>(lhs));
    const auto unsigned_rhs = static_cast<int64_t>(static_cast<uint64_t>(rhs));
    const auto product = signed_lhs * unsigned_rhs;
    return static_cast<uint32_t>(static_cast<uint64_t>(product) >> 32);
}

inline uint32_t UnsignedMulHigh(uint32_t lhs, uint32_t rhs) {
    const auto product = static_cast<uint64_t>(lhs) * static_cast<uint64_t>(rhs);
    return static_cast<uint32_t>(product >> 32);
}

inline uint32_t DivSigned(uint32_t lvalue, uint32_t rvalue) {
    const auto dividend = static_cast<int32_t>(lvalue);
    const auto divisor = static_cast<int32_t>(rvalue);

    if (divisor == 0) {
        return static_cast<uint32_t>(-1);
    }

    if (dividend == std::numeric_limits<int32_t>::min() && divisor == -1) {
        return static_cast<uint32_t>(dividend);
    }

    return static_cast<uint32_t>(dividend / divisor);
}

inline uint32_t DivUnsigned(uint32_t lvalue, uint32_t rvalue) {
    if (rvalue == 0) {
        return std::numeric_limits<uint32_t>::max();
    }

    return lvalue / rvalue;
}

inline uint32_t RemSigned(uint32_t lvalue, uint32_t rvalue) {
    const auto dividend = static_cast<int32_t>(lvalue);
    const auto divisor = static_cast<int32_t>(rvalue);

    if (divisor == 0) {
        return static_cast<uint32_t>(dividend);
    }

    if (dividend == std::numeric_limits<int32_t>::min() && divisor == -1) {
        return 0;
    }

    return static_cast<uint32_t>(dividend % divisor);
}

inline uint32_t RemUnsigned(uint32_t lvalue, uint32_t rvalue) {
    if (rvalue == 0) {
        return lvalue;
    }

    return lvalue % rvalue;
}

} // namespace

constexpr uint32_t kMulFunct7 = 0b0000001u;

struct MulOp {
    constexpr static const char* const name = "mul";
    static uint32_t exec(uint32_t lhs, uint32_t rhs) {
        return SignedMulLower(lhs, rhs);
    }
    static constexpr uint32_t funct3 = 0b000u;
    static constexpr uint32_t funct7 = kMulFunct7;
};

struct MulhOp {
    constexpr static const char* const name = "mulh";
    static uint32_t exec(uint32_t lhs, uint32_t rhs) {
        return SignedMulHigh(lhs, rhs);
    }
    static constexpr uint32_t funct3 = 0b001u;
    static constexpr uint32_t funct7 = kMulFunct7;
};

struct MulhsuOp {
    constexpr static const char* const name = "mulhsu";
    static uint32_t exec(uint32_t lhs, uint32_t rhs) {
        return SignedUnsignedMulHigh(lhs, rhs);
    }
    static constexpr uint32_t funct3 = 0b010u;
    static constexpr uint32_t funct7 = kMulFunct7;
};

struct MulhuOp {
    constexpr static const char* const name = "mulhu";
    static uint32_t exec(uint32_t lhs, uint32_t rhs) {
        return UnsignedMulHigh(lhs, rhs);
    }
    static constexpr uint32_t funct3 = 0b011u;
    static constexpr uint32_t funct7 = kMulFunct7;
};

struct DivOp {
    constexpr static const char* const name = "div";
    static uint32_t exec(uint32_t lhs, uint32_t rhs) {
        return DivSigned(lhs, rhs);
    }
    static constexpr uint32_t funct3 = 0b100u;
    static constexpr uint32_t funct7 = kMulFunct7;
};

struct DivuOp {
    constexpr static const char* const name = "divu";
    static uint32_t exec(uint32_t lhs, uint32_t rhs) {
        return DivUnsigned(lhs, rhs);
    }
    static constexpr uint32_t funct3 = 0b101u;
    static constexpr uint32_t funct7 = kMulFunct7;
};

struct RemOp {
    constexpr static const char* const name = "rem";
    static uint32_t exec(uint32_t lhs, uint32_t rhs) {
        return RemSigned(lhs, rhs);
    }
    static constexpr uint32_t funct3 = 0b110u;
    static constexpr uint32_t funct7 = kMulFunct7;
};

struct RemuOp {
    constexpr static const char* const name = "remu";
    static uint32_t exec(uint32_t lhs, uint32_t rhs) {
        return RemUnsigned(lhs, rhs);
    }
    static constexpr uint32_t funct3 = 0b111u;
    static constexpr uint32_t funct7 = kMulFunct7;
};

using Mul   = InstructionTypeR<MulOp>;
using Mulh  = InstructionTypeR<MulhOp>;
using Mulhsu = InstructionTypeR<MulhsuOp>;
using Mulhu = InstructionTypeR<MulhuOp>;
using Div   = InstructionTypeR<DivOp>;
using Divu  = InstructionTypeR<DivuOp>;
using Rem   = InstructionTypeR<RemOp>;
using Remu  = InstructionTypeR<RemuOp>;

inline void RegisterInstructionsTypeR(rvi::InstructionRegistry* registry) {
    registry->RegisterInstruction(std::make_unique<Mul>());
    registry->RegisterInstruction(std::make_unique<Mulh>());
    registry->RegisterInstruction(std::make_unique<Mulhsu>());
    registry->RegisterInstruction(std::make_unique<Mulhu>());
    registry->RegisterInstruction(std::make_unique<Div>());
    registry->RegisterInstruction(std::make_unique<Divu>());
    registry->RegisterInstruction(std::make_unique<Rem>());
    registry->RegisterInstruction(std::make_unique<Remu>());
}

} // namespace rv32m
} // namespace rvi
