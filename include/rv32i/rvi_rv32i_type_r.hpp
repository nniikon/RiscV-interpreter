// R-type arithmetic operations (add, sub, sll, srl, sra)
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

    ExecutionStatus Execute(InterpreterState* state) const override {
        auto info = std::get<InstructionDecodedInfoTypeR>(info_);
        state->regs[info.rd] = Oper::exec(state->regs[info.rs1],
                                          state->regs[info.rs2]);
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

    static constexpr uint32_t funct3 = 0x0u;
    static constexpr uint32_t funct7 = 0x00u;
};

struct SubOp {
    constexpr static const char* const name = "sub";

    static uint32_t exec(uint32_t a, uint32_t b) {
        return a - b; 
    };

    static constexpr uint32_t funct3 = 0x0u;
    static constexpr uint32_t funct7 = 0x20u;
};

struct SllOp {
    constexpr static const char* const name = "sll";

    static uint32_t exec(uint32_t a, uint32_t b) {
        return a << (b & 31);
    }

    static constexpr uint32_t funct3 = 0x1u;
    static constexpr uint32_t funct7 = 0x00u;
};

struct SrlOp {
    constexpr static const char* const name = "srl";

    static uint32_t exec(uint32_t a, uint32_t b) {
        return a >> (b & 31);
    }

    static constexpr uint32_t funct3 = 0x5u;
    static constexpr uint32_t funct7 = 0x00u;
};

struct SraOp {
    constexpr static const char* const name = "sra";

    static uint32_t exec(uint32_t a, uint32_t b) {
        return static_cast<uint32_t>( static_cast<int32_t>(a) >> (b & 31) );
    }

    static constexpr uint32_t funct3 = 0x5u;
    static constexpr uint32_t funct7 = 0x20u;
};

using Add = InstructionTypeR<AddOp>;
using Sub = InstructionTypeR<SubOp>;
using Sll = InstructionTypeR<SllOp>;
using Srl = InstructionTypeR<SrlOp>;
using Sra = InstructionTypeR<SraOp>;

namespace {

inline void RegisterInstructionsTypeR(rvi::InstructionRegistry* registry) {
    registry->RegisterInstruction(std::make_unique<Add>()); 
    registry->RegisterInstruction(std::make_unique<Sub>());
    registry->RegisterInstruction(std::make_unique<Sll>()); 
    registry->RegisterInstruction(std::make_unique<Srl>());
    registry->RegisterInstruction(std::make_unique<Sra>());
}

inline uint32_t KeyTypeR(InstructionDecodedCommonType info) {
    auto r = std::get<InstructionDecodedInfoTypeR>(info);
    return (r.funct3 << 1) | (r.funct7 >> 5);
}

} // namespace

inline void RegisterOpcodeGroupTypeR_Arithm(rvi::InstructionRegistry* registry) {
    registry->RegisterGroup(rvi::PerOpcodeGroup(/*size*/ 16u, &KeyTypeR, &DecodeInstructionToCommonTypeR),
                            Add::kOpcode);

    RegisterInstructionsTypeR(registry);
}

} // namespace rv32i
} // namespace rvi
