#pragma once

#include "rvi_instruction_interface.hpp"
#include "rvi_instruction_registry.hpp"

namespace rvi {
namespace rv32i {

template <class Oper>
class Branch : public IInstructionTypeB {
public:
    ExecutionStatus Execute(InterpreterState* state) override {
        auto lhs = static_cast<typename Oper::type>(state->regs[info_.rs1]);
        auto rhs = static_cast<typename Oper::type>(state->regs[info_.rs2]);

        if (Oper::Condition(lhs, rhs)) {
            state->pc += info_.imm;
        }

        return ExecutionStatus::Success;
    }

    const char* GetName()           const override { return Oper::name; }
    uint32_t    GetExtendedOpcode() const override { return Oper::extended_opcode; }
};

struct BeqOper {
    using type = int32_t;
    static bool Condition(type lhs, type rhs) { return lhs == rhs; }
    static constexpr const char* const name = "beq";
    static const uint32_t extended_opcode   = 0x63u;   // funct3=000, opcode=0x63
};

struct BneOper {
    using type = int32_t;
    static bool Condition(type lhs, type rhs) { return lhs != rhs; }
    static constexpr const char* const name = "bne";
    static const uint32_t extended_opcode   = 0xE3u;   // funct3=001, opcode=0x63
};

struct BltOper {
    using type = int32_t;
    static bool Condition(type lhs, type rhs) { return lhs < rhs; }
    static constexpr const char* const name = "blt";
    static const uint32_t extended_opcode   = 0x263u;  // funct3=100, opcode=0x63
};

struct BgeOper {
    using type = int32_t;
    static bool Condition(type lhs, type rhs) { return lhs >= rhs; }
    static constexpr const char* const name = "bge";
    static const uint32_t extended_opcode   = 0x2E3u;  // funct3=101, opcode=0x63
};

struct BltuOper {
    using type = uint32_t;
    static bool Condition(type lhs, type rhs) { return lhs < rhs; }
    static constexpr const char* const name = "bltu";
    static const uint32_t extended_opcode   = 0x363u;  // funct3=110, opcode=0x63
};

struct BgeuOper {
    using type = uint32_t;
    static bool Condition(type lhs, type rhs) { return lhs >= rhs; }
    static constexpr const char* const name = "bgeu";
    static const uint32_t extended_opcode   = 0x3E3u;  // funct3=111, opcode=0x63
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
