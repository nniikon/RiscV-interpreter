// R-type arithmetic and logical operations (add, sub, sll, slt, sltu, xor, srl, sra, or, and)
#pragma once

#include <cstdint>
#include <memory>

#include "rvi_decode_info.hpp"
#include "rvi_instruction_interface.hpp"
#include "rvi_instruction_registry.hpp"

namespace rvi {
namespace rv32i {

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

struct AddOp {
    constexpr static const char* const name = "add";

    static uint32_t exec(uint32_t a, uint32_t b) {
        return a + b;
    }

    static constexpr uint32_t funct3 = 0b000u;
    static constexpr uint32_t funct7 = 0x00u;
};

struct SubOp {
    constexpr static const char* const name = "sub";

    static uint32_t exec(uint32_t a, uint32_t b) {
        return a - b;
    }

    static constexpr uint32_t funct3 = 0b000u;
    static constexpr uint32_t funct7 = 0x20u;
};

struct SllOp {
    constexpr static const char* const name = "sll";

    static uint32_t exec(uint32_t a, uint32_t b) {
        return a << (b & 31u);
    }

    static constexpr uint32_t funct3 = 0b001u;
    static constexpr uint32_t funct7 = 0x00u;
};

struct SltOp {
    constexpr static const char* const name = "slt";

    static uint32_t exec(uint32_t a, uint32_t b) {
        return static_cast<int32_t>(a) < static_cast<int32_t>(b) ? 1u : 0u;
    }

    static constexpr uint32_t funct3 = 0b010u;
    static constexpr uint32_t funct7 = 0x00u;
};

struct SltuOp {
    constexpr static const char* const name = "sltu";

    static uint32_t exec(uint32_t a, uint32_t b) {
        return a < b ? 1u : 0u;
    }

    static constexpr uint32_t funct3 = 0b011u;
    static constexpr uint32_t funct7 = 0x00u;
};

struct XorOp {
    constexpr static const char* const name = "xor";

    static uint32_t exec(uint32_t a, uint32_t b) {
        return a ^ b;
    }

    static constexpr uint32_t funct3 = 0b100u;
    static constexpr uint32_t funct7 = 0x00u;
};

struct SrlOp {
    constexpr static const char* const name = "srl";

    static uint32_t exec(uint32_t a, uint32_t b) {
        return a >> (b & 31u);
    }

    static constexpr uint32_t funct3 = 0b101u;
    static constexpr uint32_t funct7 = 0x00u;
};

struct SraOp {
    constexpr static const char* const name = "sra";

    static uint32_t exec(uint32_t a, uint32_t b) {
        return static_cast<uint32_t>(static_cast<int32_t>(a) >> (b & 31u));
    }

    static constexpr uint32_t funct3 = 0b101u;
    static constexpr uint32_t funct7 = 0x20u;
};

struct OrOp {
    constexpr static const char* const name = "or";

    static uint32_t exec(uint32_t a, uint32_t b) {
        return a | b;
    }

    static constexpr uint32_t funct3 = 0b110u;
    static constexpr uint32_t funct7 = 0x00u;
};

struct AndOp {
    constexpr static const char* const name = "and";

    static uint32_t exec(uint32_t a, uint32_t b) {
        return a & b;
    }

    static constexpr uint32_t funct3 = 0b111u;
    static constexpr uint32_t funct7 = 0x00u;
};

using Add  = InstructionTypeR<AddOp>;
using Sub  = InstructionTypeR<SubOp>;
using Sll  = InstructionTypeR<SllOp>;
using Slt  = InstructionTypeR<SltOp>;
using Sltu = InstructionTypeR<SltuOp>;
using Xor  = InstructionTypeR<XorOp>;
using Srl  = InstructionTypeR<SrlOp>;
using Sra  = InstructionTypeR<SraOp>;
using Or   = InstructionTypeR<OrOp>;
using And  = InstructionTypeR<AndOp>;

inline void RegisterInstructionsTypeR(rvi::InstructionRegistry* registry) {
    registry->RegisterInstruction(std::make_unique<Add>());
    registry->RegisterInstruction(std::make_unique<Sub>());
    registry->RegisterInstruction(std::make_unique<Sll>());
    registry->RegisterInstruction(std::make_unique<Slt>());
    registry->RegisterInstruction(std::make_unique<Sltu>());
    registry->RegisterInstruction(std::make_unique<Xor>());
    registry->RegisterInstruction(std::make_unique<Srl>());
    registry->RegisterInstruction(std::make_unique<Sra>());
    registry->RegisterInstruction(std::make_unique<Or>());
    registry->RegisterInstruction(std::make_unique<And>());
}

namespace {

constexpr uint32_t kFunct3Bits = 3u;
constexpr uint32_t kFunct7Bits = 7u;
constexpr uint32_t kFunct3Mask = (1u << kFunct3Bits) - 1u;
constexpr uint32_t kFunct7Mask = (1u << kFunct7Bits) - 1u;
constexpr size_t   kOpcodeGroupKeySpace = 1u << (kFunct3Bits + kFunct7Bits);

inline uint32_t KeyTypeR(InstructionDecodedCommonType info) {
    const auto& r = std::get<InstructionDecodedInfoTypeR>(info);
    return ((r.funct7 & kFunct7Mask) << kFunct3Bits) | (r.funct3 & kFunct3Mask);
}

} // namespace

inline void RegisterOpcodeGroupTypeR_Arithm(rvi::InstructionRegistry* registry) {
    registry->RegisterGroup(rvi::PerOpcodeGroup(kOpcodeGroupKeySpace, &KeyTypeR, &DecodeInstructionToCommonTypeR),
                            Add::kOpcode);

    RegisterInstructionsTypeR(registry);
}

} // namespace rv32i
} // namespace rvi
