#include "rvi_decode_info.hpp"

#include <cassert>
#include <cstdint>

namespace rvi {

//================| Helpers |=================

static inline constexpr uint32_t bit_mask(unsigned width) {
    return ((1u << width) - 1u);
}

static inline constexpr uint32_t bits(uint32_t x, unsigned hi, unsigned lo) {
    const unsigned width = hi - lo + 1;
    return (x >> lo) & bit_mask(width);
}

static inline constexpr int32_t sex(uint32_t x, unsigned width) {
    return static_cast<int32_t>(static_cast<int32_t>(x << (32 - width)) >> (32 - width));
}

static inline constexpr uint32_t pack_ext(uint32_t f7, uint32_t f3, uint32_t opc) {
    return ((f7 & 0x7Fu) << 10) | ((f3 & 0x7u) << 7) | (opc & 0x7Fu);
}

//================| Extended Opcode Evals |=================

constexpr uint32_t EvalExtendedOpcode(InstructionDecodedInfoTypeR instr) { return pack_ext(instr.funct7, instr.funct3, instr.opcode); }

constexpr uint32_t EvalExtendedOpcode(InstructionDecodedInfoTypeI instr) {
    // FUCK YOU SRAI
    const bool is_op_imm = (instr.opcode == 0x13u);
    const bool is_shift  = (instr.funct3 == 0x1u) || (instr.funct3 == 0x5u);
    const uint32_t f7eq  = (is_op_imm && is_shift)
        ? (static_cast<uint32_t>(instr.imm) >> 5) & 0x7Fu
        : 0u;
    return pack_ext(f7eq, instr.funct3, instr.opcode);
}

constexpr uint32_t EvalExtendedOpcode(InstructionDecodedInfoTypeS instr) { return pack_ext(0u,           instr.funct3, instr.opcode); }
constexpr uint32_t EvalExtendedOpcode(InstructionDecodedInfoTypeU instr) { return pack_ext(0u,           0u,           instr.opcode); }
constexpr uint32_t EvalExtendedOpcode(InstructionDecodedInfoTypeB instr) { return pack_ext(0u,           instr.funct3, instr.opcode); }
constexpr uint32_t EvalExtendedOpcode(InstructionDecodedInfoTypeJ instr) { return pack_ext(0u,           0u,           instr.opcode); }

//================| Decoders |=================

constexpr InstructionDecodedInfoTypeR DecodeInstructionTypeR(uint32_t instr) {
    return {
        bits(instr, 6,  0),
        bits(instr, 11, 7),
        bits(instr, 14, 12),
        bits(instr, 19, 15),
        bits(instr, 24, 20),
        bits(instr, 31, 25)
    };
}

constexpr InstructionDecodedInfoTypeI DecodeInstructionTypeI(uint32_t instr) {
    const uint32_t imm12 = bits(instr, 31, 20);
    return {
        bits(instr, 6,  0),
        bits(instr, 11, 7),
        bits(instr, 14, 12),
        bits(instr, 19, 15),
        sex(imm12, 12)
    };
}

constexpr InstructionDecodedInfoTypeS DecodeInstructionTypeS(uint32_t instr) {
    const uint32_t imm11_5 = bits(instr, 31, 25);
    const uint32_t imm4_0  = bits(instr, 11, 7);
    const uint32_t imm12   = (imm11_5 << 5) | imm4_0;
    return {
        bits(instr, 6,  0),
        bits(instr, 14, 12),
        bits(instr, 19, 15),
        bits(instr, 24, 20),
        sex(imm12, 12)
    };
}

constexpr InstructionDecodedInfoTypeU DecodeInstructionTypeU(uint32_t instr) {
    const uint32_t upper_u = bits(instr, 31, 12);
    const int32_t  upper_i = static_cast<int32_t>(upper_u << 12);
    return {
        bits(instr, 6, 0),
        bits(instr, 11, 7),
        upper_i
    };
}

constexpr InstructionDecodedInfoTypeB DecodeInstructionTypeB(uint32_t instr) {
    const uint32_t imm12   = bits(instr, 31, 31);
    const uint32_t imm10_5 = bits(instr, 30, 25);
    const uint32_t imm4_1  = bits(instr, 11, 8);
    const uint32_t imm11   = bits(instr, 7, 7);
    const uint32_t imm13   = (imm12 << 12) | (imm11 << 11) | (imm10_5 << 5) | (imm4_1 << 1);
    return {
        bits(instr, 6,  0),
        bits(instr, 14, 12),
        bits(instr, 19, 15),
        bits(instr, 24, 20),
        sex(imm13, 13)
    };
}

constexpr InstructionDecodedInfoTypeJ DecodeInstructionTypeJ(uint32_t instr) {
    const uint32_t imm20    = bits(instr, 31, 31);
    const uint32_t imm10_1  = bits(instr, 30, 21);
    const uint32_t imm11    = bits(instr, 20, 20);
    const uint32_t imm19_12 = bits(instr, 19, 12);
    const uint32_t imm21    = (imm20 << 20) | (imm19_12 << 12) | (imm11 << 11) | (imm10_1 << 1);
    return {
        bits(instr, 6,  0),
        bits(instr, 11, 7),
        sex(imm21, 21)
    };
}

} // namespace