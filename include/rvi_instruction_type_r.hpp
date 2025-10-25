#pragma once

#include <cstdint>

#include "rvi_instruction_interface.hpp"
#include "rvi_instruction_registry.hpp"

namespace rvi {
namespace rv32i {

template <class Oper>
class InstructionTypeR : public IInstructionTypeR {
public:
    ExecutionStatus Execute(InterpreterState* state) override {
        state->regs[info_.rd] = Oper::exec(state->regs[info_.rs1],
                                           state->regs[info_.rs2]);
        state->pc += 4u;

        return ExecutionStatus::Success;
    }

    const char* GetName()           const override { return Oper::name; }
    uint32_t    GetExtendedOpcode() const override { return Oper::extended_opcode; }
};

struct AddOp {
    constexpr static const char* const name = "add";

    static uint32_t exec(uint32_t a, uint32_t b) {
        return a + b;
    }

    // funct7=0x00, funct3=0x0, opcode=0x33
    static const uint32_t extended_opcode = 0x00000033u;
};

struct SubOp {
    constexpr static const char* const name = "sub";

    static uint32_t exec(uint32_t a, uint32_t b) {
        return a - b; 
    };

    // funct7=0x20, funct3=0x0, opcode=0x33
    static const uint32_t extended_opcode = 0x00008033u;
};

struct SllOp {
    constexpr static const char* const name = "sll";

    static uint32_t exec(uint32_t a, uint32_t b) {
        return a << (b & 31);
    }

    // funct7=0x00, funct3=0x1, opcode=0x33
    static const uint32_t extended_opcode = 0x000000B3u;
};

struct SrlOp {
    constexpr static const char* const name = "srl";

    static uint32_t exec(uint32_t a, uint32_t b) {
        return a >> (b & 31);
    }

    // funct7=0x00, funct3=0x5, opcode=0x33
    static const uint32_t extended_opcode = 0x000002B3u;
};

struct SraOp {
    constexpr static const char* const name = "sra";

    static uint32_t exec(uint32_t a, uint32_t b) {
        return static_cast<uint32_t>( static_cast<int32_t>(a) >> (b & 31) );
    }

    // funct7=0x20, funct3=0x5, opcode=0x33
    static const uint32_t extended_opcode = 0x000082B3u;
};
    
using Add = InstructionTypeR<AddOp>;
using Sub = InstructionTypeR<SubOp>;
using Sll = InstructionTypeR<SllOp>;
using Srl = InstructionTypeR<SrlOp>;
using Sra = InstructionTypeR<SraOp>;

void RegisterInstructionsTypeR(rvi::InstructionRegistry* registry) {
    registry->RegisterInstruction(std::make_unique<Add>()); 
    registry->RegisterInstruction(std::make_unique<Sub>());
    registry->RegisterInstruction(std::make_unique<Sll>()); 
    registry->RegisterInstruction(std::make_unique<Srl>());
    registry->RegisterInstruction(std::make_unique<Sra>());
}

} // namespace rv32i
} // namespace rvi
