#pragma once

#include "rvi_decode_info.hpp"
#include "rvi_instruction_interface.hpp"
#include "rvi_instruction_registry.hpp"
#include <cstdint>

namespace rvi {
namespace rv32i {

template <class Oper>
class Branch : public IInstruction {
    static const uint32_t kOpcode = 0x63u;

public:

    ExecutionStatus Execute(InterpreterState* state) override {
        auto info = std::get<InstructionDecodedInfoTypeB>(info_);

        auto lhs = static_cast<typename Oper::type>(state->regs[info.rs1]);
        auto rhs = static_cast<typename Oper::type>(state->regs[info.rs2]);

        if (Oper::Condition(lhs, rhs)) {
            state->pc += info.imm;
        }

        return ExecutionStatus::Success;
    }

    const char* GetName()   const override { return Oper::name; }
    uint32_t    GetOpcode() const override { return kOpcode; }

    InstructionDecodedCommonType GetDecodedInfo() const override {
        InstructionDecodedInfoTypeB info = {
            .opcode = kOpcode,
            .funct3 = Oper::funct3,
        };

        return info;
    }
};

struct BeqOper {
    using type = int32_t;
    static bool Condition(type lhs, type rhs) { return lhs == rhs; }
    static constexpr const char* const name = "beq";
    static const uint32_t funct3 = 0x00u;
};

struct BneOper {
    using type = int32_t;
    static bool Condition(type lhs, type rhs) { return lhs != rhs; }
    static constexpr const char* const name = "bne";
    static const uint32_t funct3 = 0b001u;
};

struct BltOper {
    using type = int32_t;
    static bool Condition(type lhs, type rhs) { return lhs < rhs; }
    static constexpr const char* const name = "blt";
    static const uint32_t funct3 = 0b100u;
};

struct BgeOper {
    using type = int32_t;
    static bool Condition(type lhs, type rhs) { return lhs >= rhs; }
    static constexpr const char* const name = "bge";
    static const uint32_t funct3 = 0b101u;
};

struct BltuOper {
    using type = uint32_t;
    static bool Condition(type lhs, type rhs) { return lhs < rhs; }
    static constexpr const char* const name = "bltu";
    static const uint32_t funct3 = 0b110u;
};

struct BgeuOper {
    using type = uint32_t;
    static bool Condition(type lhs, type rhs) { return lhs >= rhs; }
    static constexpr const char* const name = "bgeu";
    static const uint32_t funct3 = 0b111u;
};

using Beq  = Branch<BeqOper>;
using Bne  = Branch<BneOper>;
using Blt  = Branch<BltOper>;
using Bge  = Branch<BgeOper>;
using Bltu = Branch<BltuOper>;
using Bgeu = Branch<BgeuOper>;

void RegisterInstructionsTypeB(InstructionRegistry* registry) {
    registry->RegisterInstruction(std::make_unique<Beq> ());
    registry->RegisterInstruction(std::make_unique<Bne> ());
    registry->RegisterInstruction(std::make_unique<Blt> ());
    registry->RegisterInstruction(std::make_unique<Bge> ());
    registry->RegisterInstruction(std::make_unique<Bltu>());
    registry->RegisterInstruction(std::make_unique<Bgeu>()); 
}

} // namespace rv32i
} // namespace rvi
