// implements

#pragma once

#include <bit>
#include <cfenv>
#include <cmath>
#include <cstdint>
#include <limits>
#include <memory>

#include "rvi_decode_info.hpp"
#include "rvi_instruction_interface.hpp"
#include "rvi_instruction_registry.hpp"

#include "loguru.hpp"

namespace rvi {
namespace rv32f {

namespace {

enum class RoundingMode : uint32_t {
    kRNE = 0,
    kRTZ,
    kRDN,
    kRUP,
    kRMM,
};

inline RoundingMode DecodeRoundingMode(uint32_t rm) {
    switch (rm & 0x7u) {
        case 0b000u: return RoundingMode::kRNE;
        case 0b001u: return RoundingMode::kRTZ;
        case 0b010u: return RoundingMode::kRDN;
        case 0b011u: return RoundingMode::kRUP;
        case 0b100u: return RoundingMode::kRMM;
        case 0b111u:
        default:     return RoundingMode::kRNE;
    }
}

inline int ToFERoundingMode(RoundingMode mode) {
    switch (mode) {
        case RoundingMode::kRNE: return FE_TONEAREST;
        case RoundingMode::kRTZ: return FE_TOWARDZERO;
        case RoundingMode::kRDN: return FE_DOWNWARD;
        case RoundingMode::kRUP: return FE_UPWARD;
        case RoundingMode::kRMM: return FE_TONEAREST;
        default: assert(0);      return FE_TONEAREST;
    }
    return FE_TONEAREST;
}

class ScopedRoundingMode {
public:
    explicit ScopedRoundingMode(uint32_t rm)
        : previous_(std::fegetround()) {
        std::fesetround(ToFERoundingMode(DecodeRoundingMode(rm)));
    }

    ~ScopedRoundingMode() {
        std::fesetround(previous_);
    }

private:
    int previous_;
};

template <class IntType>
IntType ConvertFloatToInt(float value) {
    double rounded = std::nearbyint(static_cast<double>(value));

    const double min_value = static_cast<double>(std::numeric_limits<IntType>::min());
    const double max_value = static_cast<double>(std::numeric_limits<IntType>::max());

    if (rounded < min_value) {
        rounded = min_value;
    } else if (rounded > max_value) {
        rounded = max_value;
    }

    return static_cast<IntType>(rounded);
}

inline uint32_t ClassifyFloat(float value) {
    const uint32_t bits = std::bit_cast<uint32_t>(value);
    const bool     sign = (bits >> 31) != 0u;
    const uint32_t exponent = (bits >> 23) & 0xFFu;
    const uint32_t fraction = bits & 0x7FFFFFu;

    const bool exp_is_zero = (exponent == 0u);
    const bool exp_is_max  = (exponent == 0xFFu);
    const bool frac_is_zero = (fraction == 0u);

    if (exp_is_max && frac_is_zero) {
        return sign ? (1u << 0) : (1u << 7);
    }

    if (exp_is_max && !frac_is_zero) {
        const bool is_signaling = (fraction & (1u << 22)) == 0u;
        return is_signaling ? (1u << 8) : (1u << 9);
    }

    if (exp_is_zero && frac_is_zero) {
        return sign ? (1u << 3) : (1u << 4);
    }

    if (exp_is_zero && !frac_is_zero) {
        return sign ? (1u << 2) : (1u << 5);
    }

    return sign ? (1u << 1) : (1u << 6);
}

inline float MakeFloatWithSign(float magnitude_src, float sign_src) {
    const uint32_t magnitude = std::bit_cast<uint32_t>(magnitude_src) & 0x7FFF'FFFFu;
    const uint32_t sign      = std::bit_cast<uint32_t>(sign_src) & 0x8000'0000u;
    return std::bit_cast<float>(magnitude | sign);
}

} // namespace

template <class Oper>
class InstructionTypeRF final : public IInstruction {
public:
    static constexpr uint32_t kOpcode = 0x53u;

    ExecutionStatus Execute(InterpreterState* state,
                            const InstructionDecodedCommonType& decoded_info) const override {
        const auto& info = std::get<InstructionDecodedInfoTypeR>(decoded_info);

        ScopedRoundingMode guard(info.funct7 & 0x03);
        Oper::exec(state, info);

        LOG_F(INFO, "HUI %s(%f, %f) = %f", Oper::name, state->f_regs.Get(info.rs1), state->f_regs.Get(info.rs2), state->f_regs.Get(info.rd));

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

struct FAddSOper {
    constexpr static const char* const name = "fadd.s";
    constexpr static uint32_t funct7 = 0b0000000u;
    constexpr static uint32_t funct3 = 0b111;

    static void exec(InterpreterState* state, const InstructionDecodedInfoTypeR& info) {
        const float lhs = state->f_regs.Get(info.rs1);
        const float rhs = state->f_regs.Get(info.rs2);

        state->f_regs.Set(info.rd, lhs + rhs);
    }
};

struct FSubSOper {
    constexpr static const char* const name = "fsub.s";
    constexpr static uint32_t funct7 = 0b0000100u;
    constexpr static uint32_t funct3 = 0b111;

    static void exec(InterpreterState* state, const InstructionDecodedInfoTypeR& info) {
        const float lhs = state->f_regs.Get(info.rs1);
        const float rhs = state->f_regs.Get(info.rs2);

        state->f_regs.Set(info.rd, lhs - rhs);
    }
};

struct FMulSOper {
    constexpr static const char* const name = "fmul.s";
    constexpr static uint32_t funct7 = 0b0001000u;
    constexpr static uint32_t funct3 = 0b111;

    static void exec(InterpreterState* state, const InstructionDecodedInfoTypeR& info) {
        const float lhs = state->f_regs.Get(info.rs1);
        const float rhs = state->f_regs.Get(info.rs2);

        state->f_regs.Set(info.rd, lhs * rhs);
    }
};

struct FDivSOper {
    constexpr static const char* const name = "fdiv.s";
    constexpr static uint32_t funct7 = 0b0001100u;
    constexpr static uint32_t funct3 = 0b111;

    static void exec(InterpreterState* state, const InstructionDecodedInfoTypeR& info) {
        const float lhs = state->f_regs.Get(info.rs1);
        const float rhs = state->f_regs.Get(info.rs2);

        state->f_regs.Set(info.rd, lhs / rhs);
    }
};

struct FSqrtSOper {
    constexpr static const char* const name = "fsqrt.s";
    constexpr static uint32_t funct7 = 0b0101100u;
    constexpr static uint32_t funct3 = 0b111;

    static void exec(InterpreterState* state, const InstructionDecodedInfoTypeR& info) {
        const float value = state->f_regs.Get(info.rs1);

        state->f_regs.Set(info.rd, std::sqrt(value));
    }
};

struct FSgnjSOper {
    constexpr static const char* const name = "fsgnj.s";
    constexpr static uint32_t funct7 = 0b0010000u;
    constexpr static uint32_t funct3 = 0b000u;

    static void exec(InterpreterState* state, const InstructionDecodedInfoTypeR& info) {
        const float lhs = state->f_regs.Get(info.rs1);
        const float rhs = state->f_regs.Get(info.rs2);

        state->f_regs.Set(info.rd, MakeFloatWithSign(lhs, rhs));
    }
};

struct FSgnjnSOper {
    constexpr static const char* const name = "fsgnjn.s";
    constexpr static uint32_t funct7 = 0b0010000u;
    constexpr static uint32_t funct3 = 0b001u;

    static void exec(InterpreterState* state, const InstructionDecodedInfoTypeR& info) {
        const float lhs = state->f_regs.Get(info.rs1);
        float       rhs = state->f_regs.Get(info.rs2);

        const uint32_t inverted_sign = std::bit_cast<uint32_t>(rhs) ^ 0x8000'0000u;
        rhs = std::bit_cast<float>(inverted_sign);
        state->f_regs.Set(info.rd, MakeFloatWithSign(lhs, rhs));
    }
};

struct FSgnjxSOper {
    constexpr static const char* const name = "fsgnjx.s";
    constexpr static uint32_t funct7 = 0b0010000u;
    constexpr static uint32_t funct3 = 0b010u;

    static void exec(InterpreterState* state, const InstructionDecodedInfoTypeR& info) {
        const uint32_t lhs_bits = std::bit_cast<uint32_t>(state->f_regs.Get(info.rs1));
        const uint32_t rhs_bits = std::bit_cast<uint32_t>(state->f_regs.Get(info.rs2));

        const uint32_t magnitude = lhs_bits & 0x7FFF'FFFFu;
        const uint32_t sign      = (lhs_bits ^ rhs_bits) & 0x8000'0000u;

        state->f_regs.Set(info.rd, std::bit_cast<float>(magnitude | sign));
    }
};

struct FMinSOper {
    constexpr static const char* const name = "fmin.s";
    constexpr static uint32_t funct7 = 0b0010100u;
    constexpr static uint32_t funct3 = 0b000u;

    static void exec(InterpreterState* state, const InstructionDecodedInfoTypeR& info) {
        const float lhs = state->f_regs.Get(info.rs1);
        const float rhs = state->f_regs.Get(info.rs2);

        state->f_regs.Set(info.rd, std::fmin(lhs, rhs));
    }
};

struct FMaxSOper {
    constexpr static const char* const name = "fmax.s";
    constexpr static uint32_t funct7 = 0b0010100u;
    constexpr static uint32_t funct3 = 0b001u;

    static void exec(InterpreterState* state, const InstructionDecodedInfoTypeR& info) {
        const float lhs = state->f_regs.Get(info.rs1);
        const float rhs = state->f_regs.Get(info.rs2);

        state->f_regs.Set(info.rd, std::fmax(lhs, rhs));
    }
};

struct FEqSOper {
    constexpr static const char* const name = "feq.s";
    constexpr static uint32_t funct7 = 0b1010000u;
    constexpr static uint32_t funct3 = 0b010u;

    static void exec(InterpreterState* state, const InstructionDecodedInfoTypeR& info) {
        const float lhs = state->f_regs.Get(info.rs1);
        const float rhs = state->f_regs.Get(info.rs2);

        const bool  result = (!std::isnan(lhs) && !std::isnan(rhs) && lhs == rhs);

        state->regs.Set(info.rd, result ? 1u : 0u);
    }
};

struct FLtSOper {
    constexpr static const char* const name = "flt.s";
    constexpr static uint32_t funct7 = 0b1010000u;
    constexpr static uint32_t funct3 = 0b001u;

    static void exec(InterpreterState* state, const InstructionDecodedInfoTypeR& info) {
        const float lhs = state->f_regs.Get(info.rs1);
        const float rhs = state->f_regs.Get(info.rs2);

        const bool  result = (!std::isnan(lhs) && !std::isnan(rhs) && lhs < rhs);

        state->regs.Set(info.rd, result ? 1u : 0u);
    }
};

struct FLeSOper {
    constexpr static const char* const name = "fle.s";
    constexpr static uint32_t funct7 = 0b1010000u;
    constexpr static uint32_t funct3 = 0b000u;

    static void exec(InterpreterState* state, const InstructionDecodedInfoTypeR& info) {
        const float lhs = state->f_regs.Get(info.rs1);
        const float rhs = state->f_regs.Get(info.rs2);

        const bool  result = (!std::isnan(lhs) && !std::isnan(rhs) && lhs <= rhs);

        state->regs.Set(info.rd, result ? 1u : 0u);
    }
};

struct FCvtWSOper {
    constexpr static const char* const name = "fcvt.w.s";
    constexpr static uint32_t funct7 = 0b1100000u;
    constexpr static uint32_t funct3 = 0b111;

    static void exec(InterpreterState* state, const InstructionDecodedInfoTypeR& info) {
        const float   value = state->f_regs.Get(info.rs1);
        const int32_t result = ConvertFloatToInt<int32_t>(value);

        state->regs.Set(info.rd, static_cast<uint32_t>(result));
    }
};

struct FCvtWUSOper {
    constexpr static const char* const name = "fcvt.wu.s";
    constexpr static uint32_t funct7 = 0b1100000u;
    constexpr static uint32_t funct3 = 111u;

    static void exec(InterpreterState* state, const InstructionDecodedInfoTypeR& info) {
        const float    value = state->f_regs.Get(info.rs1);
        const uint32_t result = ConvertFloatToInt<uint32_t>(value);

        state->regs.Set(info.rd, result);
    }
};

struct FCvtSWOper {
    constexpr static const char* const name = "fcvt.s.w";
    constexpr static uint32_t funct7 = 0b1101000u;
    constexpr static uint32_t funct3 = 111u;

    static void exec(InterpreterState* state, const InstructionDecodedInfoTypeR& info) {
        const int32_t value = static_cast<int32_t>(state->regs.Get(info.rs1));

        state->f_regs.Set(info.rd, static_cast<float>(value));
    }
};

struct FCvtSWUOper {
    constexpr static const char* const name = "fcvt.s.wu";
    constexpr static uint32_t funct7 = 0b1101000u;
    constexpr static uint32_t funct3 = 111u;

    static void exec(InterpreterState* state, const InstructionDecodedInfoTypeR& info) {
        const uint32_t value = state->regs.Get(info.rs1);

        state->f_regs.Set(info.rd, static_cast<float>(value));
    }
};

struct FClassSOper {
    constexpr static const char* const name = "fclass.s";
    constexpr static uint32_t funct7 = 0b1110000u;
    constexpr static uint32_t funct3 = 0b001u;

    static void exec(InterpreterState* state, const InstructionDecodedInfoTypeR& info) {
        const float value = state->f_regs.Get(info.rs1);

        state->regs.Set(info.rd, ClassifyFloat(value));
    }
};

struct FMvXWOper {
    constexpr static const char* const name = "fmv.x.w";
    constexpr static uint32_t funct7 = 0b1110000u;
    constexpr static uint32_t funct3 = 0b000u;

    static void exec(InterpreterState* state, const InstructionDecodedInfoTypeR& info) {
        const float value = state->f_regs.Get(info.rs1);

        state->regs.Set(info.rd, std::bit_cast<uint32_t>(value));
    }
};

struct FMvWXOper {
    constexpr static const char* const name = "fmv.w.x";
    constexpr static uint32_t funct7 = 0b1111000u;
    constexpr static uint32_t funct3 = 0b000u;

    static void exec(InterpreterState* state, const InstructionDecodedInfoTypeR& info) {
        const uint32_t value = state->regs.Get(info.rs1);

        state->f_regs.Set(info.rd, std::bit_cast<float>(value));
    }
};

using FAddS   = InstructionTypeRF<FAddSOper>;
using FSubS   = InstructionTypeRF<FSubSOper>;
using FMulS   = InstructionTypeRF<FMulSOper>;
using FDivS   = InstructionTypeRF<FDivSOper>;
using FSqrtS  = InstructionTypeRF<FSqrtSOper>;
using FSgnjS  = InstructionTypeRF<FSgnjSOper>;
using FSgnjnS = InstructionTypeRF<FSgnjnSOper>;
using FSgnjxS = InstructionTypeRF<FSgnjxSOper>;
using FMinS   = InstructionTypeRF<FMinSOper>;
using FMaxS   = InstructionTypeRF<FMaxSOper>;
using FEqS    = InstructionTypeRF<FEqSOper>;
using FLtS    = InstructionTypeRF<FLtSOper>;
using FLeS    = InstructionTypeRF<FLeSOper>;
using FCvtWS  = InstructionTypeRF<FCvtWSOper>;
using FCvtWUS = InstructionTypeRF<FCvtWUSOper>;
using FCvtSW  = InstructionTypeRF<FCvtSWOper>;
using FCvtSWU = InstructionTypeRF<FCvtSWUOper>;
using FClassS = InstructionTypeRF<FClassSOper>;
using FMvXW   = InstructionTypeRF<FMvXWOper>;
using FMvWX   = InstructionTypeRF<FMvWXOper>;

namespace {

inline void RegisterInstructionsTypeR_Float(rvi::InstructionRegistry* registry) {
    registry->RegisterInstruction(std::make_unique<FAddS>());
    registry->RegisterInstruction(std::make_unique<FSubS>());
    registry->RegisterInstruction(std::make_unique<FMulS>());
    registry->RegisterInstruction(std::make_unique<FDivS>());
    registry->RegisterInstruction(std::make_unique<FSqrtS>());
    registry->RegisterInstruction(std::make_unique<FSgnjS>());
    registry->RegisterInstruction(std::make_unique<FSgnjnS>());
    registry->RegisterInstruction(std::make_unique<FSgnjxS>());
    registry->RegisterInstruction(std::make_unique<FMinS>());
    registry->RegisterInstruction(std::make_unique<FMaxS>());
    registry->RegisterInstruction(std::make_unique<FEqS>());
    registry->RegisterInstruction(std::make_unique<FLtS>());
    registry->RegisterInstruction(std::make_unique<FLeS>());
    registry->RegisterInstruction(std::make_unique<FCvtWS>());
    registry->RegisterInstruction(std::make_unique<FCvtWUS>());
    registry->RegisterInstruction(std::make_unique<FCvtSW>());
    registry->RegisterInstruction(std::make_unique<FCvtSWU>());
    registry->RegisterInstruction(std::make_unique<FClassS>());
    registry->RegisterInstruction(std::make_unique<FMvXW>());
    registry->RegisterInstruction(std::make_unique<FMvWX>());
}

constexpr uint32_t kFunct3Bits = 3u;
constexpr uint32_t kFunct7Bits = 7u;
constexpr uint32_t kRs2Bits = 5u;
constexpr uint32_t kFunct3Mask = (1u << kFunct3Bits) - 1u;
constexpr uint32_t kFunct7Mask = (1u << kFunct7Bits) - 1u;
constexpr uint32_t kRs2Mask = (1u << kRs2Bits) - 1u;
constexpr size_t   kOpcodeGroupKeySpace = 1u << (kFunct3Bits + kFunct7Bits + kRs2Bits);

inline bool Funct7UsesRm(uint32_t funct7) {
    switch (funct7 & kFunct7Mask) {
        case 0b0000000u:
        case 0b0000100u:
        case 0b0001000u:
        case 0b0001100u:
        case 0b0101100u:
        case 0b1100000u:
        case 0b1101000u:
            return true;
        default:
            return false;
    }
}

inline bool Funct7RequiresRs2Specifier(uint32_t funct7) {
    switch (funct7 & kFunct7Mask) {
        case 0b1100000u:
        case 0b1101000u:
            return true;
        default:
            return false;
    }
}

inline uint32_t KeyTypeR_Float(InstructionDecodedCommonType info) {
    const auto& r = std::get<InstructionDecodedInfoTypeR>(info);

    uint32_t funct7 = r.funct7 & kFunct7Mask;
    uint32_t funct3 = r.funct3 & kFunct3Mask;

    if (Funct7UsesRm(funct7)) {
        funct3 = 0u;
    }

    uint32_t key = (funct7 << kFunct3Bits) | funct3;
    key <<= kRs2Bits;

    if (Funct7RequiresRs2Specifier(funct7)) {
        key |= (r.rs2 & kRs2Mask);
    }

    return key;
}

} // namespace

inline void RegisterOpcodeGroupTypeR_Float(rvi::InstructionRegistry* registry) {
    registry->RegisterGroup(
        rvi::PerOpcodeGroup(kOpcodeGroupKeySpace, &KeyTypeR_Float, &DecodeInstructionToCommonTypeR),
        FAddS::kOpcode);

    RegisterInstructionsTypeR_Float(registry);
}

} // namespace rv32f
} // namespace rvi
