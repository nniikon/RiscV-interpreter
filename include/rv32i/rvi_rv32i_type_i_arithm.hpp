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
    auto i = std::get<InstructionDecodedInfoTypeI>(info);
    uint32_t srai_bit = 0;


    if (i.funct3 == SraiOper::funct3) {
        srai_bit = (static_cast<uint32_t>(i.imm) >> 10); // 0 for SRLI, 1 for SRAI
    }

    return static_cast<uint32_t>((i.funct3 << 1) | srai_bit);
}

} // namespace

inline void RegisterOpcodeGroupTypeI_Arithm(rvi::InstructionRegistry* registry) {
    registry->RegisterGroup(
        rvi::PerOpcodeGroup(/*size*/ 16u, &KeyTypeI_Arithm, &DecodeInstructionToCommonTypeI),
        Addi::kOpcode);

    RegisterInstructionsTypeI_Arithm(registry);
}

} // namespace rv32i
} // namespace rvi
