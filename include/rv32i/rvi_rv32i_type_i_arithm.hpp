// I-type arithmetic-immediate operations
#pragma once

#include <cstdint>
#include <memory>

#include "rvi_decode_info.hpp"
#include "rvi_instruction_interface.hpp"
#include "rvi_instruction_registry.hpp"

#include "loguru.hpp"

namespace rvi {
namespace rv32i {

template <class Oper>
class Arithm final : public IInstruction {
public:
    static constexpr uint32_t kOpcode = 0x13u;

    ExecutionStatus Execute(InterpreterState* state,
                            const InstructionDecodedCommonType& decoded_info) const override {
        const auto& info = std::get<InstructionDecodedInfoTypeI>(decoded_info);

        using lhs_type = typename Oper::value_type;
        using rhs_type = typename Oper::imm_type;

        lhs_type lhs = static_cast<lhs_type>(state->regs.Get(info.rs1));
        rhs_type rhs = Oper::GetImm(info);

        auto result = Oper::exec(lhs, rhs);
        state->regs.Set(info.rd, static_cast<uint32_t>(result));
        state->pc += 4u;

        return ExecutionStatus::Success;
    }

    const char* GetName()   const override { return Oper::name; }
    uint32_t    GetOpcode() const override { return kOpcode; }

    InstructionDecodedCommonType GetDecodedInfo() const override {
        InstructionDecodedInfoTypeI info = {
            .opcode = kOpcode,
            .funct3 = Oper::funct3,
            .imm = Oper::imm,
        };

        return info;
    }
};

namespace {

struct AddiOper {
    using value_type = int32_t;
    using imm_type = int32_t;
    constexpr static const char* const name = "addi";

    static imm_type GetImm(const InstructionDecodedInfoTypeI& info) {
        return info.imm;
    }

    static value_type exec(value_type lhs, imm_type rhs) {
        return lhs + rhs;
    }

    static constexpr uint32_t funct3 = 0b000u;
    static constexpr uint32_t imm = 0b0000'0000'0000u;
};

struct StliOper {
    using value_type = int32_t;
    using imm_type = int32_t;
    constexpr static const char* const name = "stli";

    static imm_type GetImm(const InstructionDecodedInfoTypeI& info) {
        return info.imm;
    }

    static value_type exec(value_type lhs, imm_type rhs) {
        return lhs < rhs ? 1 : 0;
    }

    static constexpr uint32_t funct3 = 0b010u;
    static constexpr uint32_t imm = 0b0000'0000'0000u;
};

struct StliuOper {
    using value_type = uint32_t;
    using imm_type = uint32_t;
    constexpr static const char* const name = "stliu";

    static imm_type GetImm(const InstructionDecodedInfoTypeI& info) {
        return static_cast<uint32_t>(info.imm);
    }

    static value_type exec(value_type lhs, imm_type rhs) {
        return lhs < rhs ? 1 : 0;
    }

    static constexpr uint32_t funct3 = 0b011u;
    static constexpr uint32_t imm = 0b0000'0000'0000u;
};

struct XoriOper {
    using value_type = uint32_t;
    using imm_type = uint32_t;
    constexpr static const char* const name = "xori";

    static imm_type GetImm(const InstructionDecodedInfoTypeI& info) {
        return static_cast<uint32_t>(info.imm);
    }

    static value_type exec(value_type lhs, imm_type rhs) {
        return lhs ^ rhs;
    }

    static constexpr uint32_t funct3 = 0b100u;
    static constexpr uint32_t imm = 0b0000'0000'0000u;
};

struct AndiOper {
    using value_type = uint32_t;
    using imm_type = uint32_t;
    constexpr static const char* const name = "andi";

    static imm_type GetImm(const InstructionDecodedInfoTypeI& info) {
        return static_cast<uint32_t>(info.imm);
    }

    static value_type exec(value_type lhs, imm_type rhs) {
        return lhs & rhs;
    }

    static constexpr uint32_t funct3 = 0b111u;
    static constexpr uint32_t imm = 0b0000'0000'0000u;
};

struct OriOper {
    using value_type = uint32_t;
    using imm_type = uint32_t;
    constexpr static const char* const name = "ori";

    static imm_type GetImm(const InstructionDecodedInfoTypeI& info) {
        return static_cast<uint32_t>(info.imm);
    }

    static value_type exec(value_type lhs, imm_type rhs) {
        return lhs | rhs;
    }

    static constexpr uint32_t funct3 = 0b110u;
    static constexpr uint32_t imm = 0b0000'0000'0000u;
};

struct SlliOper {
    using value_type = uint32_t;
    using imm_type = uint32_t;
    constexpr static const char* const name = "slli";

    static imm_type GetImm(const InstructionDecodedInfoTypeI& info) {
        return static_cast<uint32_t>(info.imm) & 0x1Fu;
    }

    static value_type exec(value_type lhs, imm_type rhs) {
        return lhs << rhs;
    }

    static constexpr uint32_t funct3 = 0b001u;
    static constexpr uint32_t imm = 0b0000'0000'0000u;
};

struct SrliOper {
    using value_type = uint32_t;
    using imm_type = uint32_t;
    constexpr static const char* const name = "srli";

    static imm_type GetImm(const InstructionDecodedInfoTypeI& info) {
        return static_cast<uint32_t>(info.imm) & 0x1Fu;
    }

    static value_type exec(value_type lhs, imm_type rhs) {
        return lhs >> rhs;
    }

    static constexpr uint32_t funct3 = 0b101u;
    static constexpr uint32_t imm = 0b0000'0000'0000u;
};

struct SraiOper {
    using value_type = int32_t;
    using imm_type = uint32_t;
    constexpr static const char* const name = "srai";

    static imm_type GetImm(const InstructionDecodedInfoTypeI& info) {
        return static_cast<uint32_t>(info.imm) & 0x1Fu;
    }

    static value_type exec(value_type lhs, imm_type rhs) {
        return lhs >> rhs;
    }

    static constexpr uint32_t funct3 = 0b101u;
    static constexpr uint32_t imm = 0b0100'0000'0000u; // FUCK YOU
};

using Srai  = Arithm<SraiOper>;
using Srli  = Arithm<SrliOper>;
using Slli  = Arithm<SlliOper>;
using Ori   = Arithm<OriOper>;
using Andi  = Arithm<AndiOper>;
using Xori  = Arithm<XoriOper>;
using Stliu = Arithm<StliuOper>;
using Stli  = Arithm<StliOper>;
using Addi  = Arithm<AddiOper>;

inline void RegisterInstructionsTypeI_Arithm(rvi::InstructionRegistry* registry) {
    registry->RegisterInstruction(std::make_unique<Srai> ()); 
    registry->RegisterInstruction(std::make_unique<Srli> ()); 
    registry->RegisterInstruction(std::make_unique<Slli> ()); 
    registry->RegisterInstruction(std::make_unique<Ori>  ());  
    registry->RegisterInstruction(std::make_unique<Andi> ()); 
    registry->RegisterInstruction(std::make_unique<Xori> ()); 
    registry->RegisterInstruction(std::make_unique<Stliu>());
    registry->RegisterInstruction(std::make_unique<Stli> ()); 
    registry->RegisterInstruction(std::make_unique<Addi> ());
}

inline uint32_t KeyTypeI_Arithm(InstructionDecodedCommonType info) {
    // FIXME: should be smarter and not specific to any instruction
    const auto i = std::get<InstructionDecodedInfoTypeI>(info);
    const uint32_t funct3 = i.funct3 & 0x7u;
    const uint32_t imm = static_cast<uint32_t>(i.imm) & 0xFFFu;

    switch (funct3) {
        case 0b000u: return 0u;  // addi
        case 0b010u: return 4u;  // slti
        case 0b011u: return 6u;  // sltiu
        case 0b100u: return 8u;  // xori
        case 0b110u: return 12u; // ori
        case 0b111u: return 14u; // andi
        case 0b001u: {
            const uint32_t imm_hi = (imm >> 5) & 0x7Fu;
            if (imm_hi == 0u) {
                return 2u; // slli
            }
            if (imm_hi == 0b0110000u) {
                const uint32_t imm_lo = imm & 0x1Fu;
                switch (imm_lo) {
                    case 0b00000u: return 16u; // clz
                    case 0b00001u: return 17u; // ctz
                    case 0b00010u: return 18u; // cpop
                    case 0b00100u: return 19u; // sext.b
                    case 0b00101u: return 20u; // sext.h
                    default: break;
                }
            }
            break;
        }
        case 0b101u: {
            const uint32_t imm_hi = (imm >> 5) & 0x7Fu;
            if (imm_hi == 0u) {
                return 10u; // srli
            }
            if (imm_hi == 0b0100000u) {
                return 11u; // srai
            }
            if (imm_hi == 0b0110000u) {
                return 21u; // rori
            }
            if (imm_hi == 0b0010100u) {
                return 22u; // orc.b
            }
            if (imm_hi == 0b0110100u) {
                return 23u; // rev8
            }
            break;
        }
        default:
            break;
    }

    uint32_t srai_bit = 0;
    if (funct3 == SraiOper::funct3) {
        srai_bit = (imm >> 10) & 0x1u; // 0 for SRLI, 1 for SRAI
    }

    return static_cast<uint32_t>((funct3 << 1) | srai_bit);
}

} // namespace

inline void RegisterOpcodeGroupTypeI_Arithm(rvi::InstructionRegistry* registry) {
    registry->RegisterGroup(
        rvi::PerOpcodeGroup(/*size*/ 32u, &KeyTypeI_Arithm, &DecodeInstructionToCommonTypeI),
        Addi::kOpcode);

    RegisterInstructionsTypeI_Arithm(registry);
}

} // namespace rv32i
} // namespace rvi
