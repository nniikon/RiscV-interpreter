// B-type BRANCH (beq, bne, blt, bge, bltu, bgeu)
#pragma once

#include <cstdint>
#include <memory>

#include "rvi_decode_info.hpp"
#include "rvi_instruction_interface.hpp"
#include "rvi_instruction_registry.hpp"

namespace rvi {
namespace rv32i {

template <class Oper>
class Branch final : public IInstruction {
public:
    static constexpr uint32_t kOpcode = 0x63u;

    ExecutionStatus Execute(InterpreterState* state) const override {
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

namespace {

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

inline void RegisterInstructionsTypeB(InstructionRegistry* registry) {
    registry->RegisterInstruction(std::make_unique<Beq> ());
    registry->RegisterInstruction(std::make_unique<Bne> ());
    registry->RegisterInstruction(std::make_unique<Blt> ());
    registry->RegisterInstruction(std::make_unique<Bge> ());
    registry->RegisterInstruction(std::make_unique<Bltu>());
    registry->RegisterInstruction(std::make_unique<Bgeu>()); 
}

inline uint32_t KeyTypeB_Branch(InstructionDecodedCommonType info) {
    auto b = std::get<InstructionDecodedInfoTypeB>(info);
    return b.funct3 & 0x7u;
}

} // namespace

inline void RegisterOpcodeGroupTypeB_Branch(rvi::InstructionRegistry* registry) {
    registry->RegisterGroup(rvi::PerOpcodeGroup(/*size*/ 8u, &KeyTypeB_Branch), 0x63u);

    RegisterInstructionsTypeB(registry);
}

} // namespace rv32i
} // namespace rvi
