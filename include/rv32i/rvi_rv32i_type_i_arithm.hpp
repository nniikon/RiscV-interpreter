// I-type arithmetic-immediate operations
#pragma once

#include <cstdint>
#include <memory>

#include "rvi_decode_info.hpp"
#include "rvi_instruction_interface.hpp"
#include "rvi_instruction_registry.hpp"

namespace rvi {
namespace rv32i {

template <class Oper>
class Arithm final : public IInstruction {
public:
    static constexpr uint32_t kOpcode = 0x13u;

    ExecutionStatus Execute(InterpreterState* state) const override {
        auto info = std::get<InstructionDecodedInfoTypeI>(info_);

        state->regs[info.rd] = Oper::exec(state->regs[info.rs1], static_cast<uint32_t>(info.imm));
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
    constexpr static const char* const name = "addi";

    static uint32_t exec(uint32_t a, uint32_t b) {
        return a + b;
    }

    static constexpr uint32_t funct3 = 0b000u;
    static constexpr uint32_t imm = 0b0000'0000'0000u;
};

struct StliOper {
    constexpr static const char* const name = "stli";

    static uint32_t exec(uint32_t a, uint32_t b) {
        auto a_i = static_cast<int32_t>(a);
        auto b_i = static_cast<int32_t>(b);
        return a_i < b_i ? 1 : 0;
    }

    static constexpr uint32_t funct3 = 0b010u;
    static constexpr uint32_t imm = 0b0000'0000'0000u;
};

struct StliuOper {
    constexpr static const char* const name = "stliu";

    static uint32_t exec(uint32_t a, uint32_t b) {
        return a < b ? 1 : 0;
    }

    static constexpr uint32_t funct3 = 0b011u;
    static constexpr uint32_t imm = 0b0000'0000'0000u;
};

struct XoriOper {
    constexpr static const char* const name = "xori";

    static uint32_t exec(uint32_t a, uint32_t b) {
        return a ^ b;
    }

    static constexpr uint32_t funct3 = 0b100u;
    static constexpr uint32_t imm = 0b0000'0000'0000u;
};

struct AndiOper {
    constexpr static const char* const name = "andi";

    static uint32_t exec(uint32_t a, uint32_t b) {
        return a & b;
    }

    static constexpr uint32_t funct3 = 0b111u;
    static constexpr uint32_t imm = 0b0000'0000'0000u;
};

struct OriOper {
    constexpr static const char* const name = "ori";

    static uint32_t exec(uint32_t a, uint32_t b) {
        return a | b;
    }

    static constexpr uint32_t funct3 = 0b110u;
    static constexpr uint32_t imm = 0b0000'0000'0000u;
};

struct SlliOper {
    constexpr static const char* const name = "slli";

    static uint32_t exec(uint32_t a, uint32_t b) {
        return a << (b & 0x1Fu);
    }

    static constexpr uint32_t funct3 = 0b001u;
    static constexpr uint32_t imm = 0b0000'0000'0000u;
};

struct SrliOper {
    constexpr static const char* const name = "srli";

    static uint32_t exec(uint32_t a, uint32_t b) {
        return a >> (b & 0x1Fu);
    }

    static constexpr uint32_t funct3 = 0b101u;
    static constexpr uint32_t imm = 0b0000'0000'0000u;
};

struct SraiOper {
    constexpr static const char* const name = "srai";

    static uint32_t exec(uint32_t a, uint32_t b) {
        return static_cast<uint32_t>(
            static_cast<int>(a) >> (b & 0x1Fu)
        );
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
        srai_bit = (static_cast<uint32_t>(i.imm) >> 11); // 0 for SRLI, 1 for SRAI
    }

    return static_cast<uint32_t>((i.funct3 << 1) | srai_bit);
}

} // namespace

inline void RegisterOpcodeGroupTypeI_Arithm(rvi::InstructionRegistry* registry) {
    registry->RegisterGroup(rvi::PerOpcodeGroup(/*size*/ 16u, &KeyTypeI_Arithm), Addi::kOpcode);

    RegisterInstructionsTypeI_Arithm(registry);
}

} // namespace rv32i
} // namespace rvi
