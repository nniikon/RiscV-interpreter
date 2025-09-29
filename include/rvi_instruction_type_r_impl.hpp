#pragma once

#include <cstdint>

#include "rvi_instruction_interface.hpp"

namespace rvi {
namespace rv32i {

template <class Oper>
class InstructionTypeR : public IInstructionTypeR {
public:
    ExecutionStatus Execute(InterpreterState* state) override {
        state->regs[info.rd] = Oper::exec(state->regs[info.rs1],
                                        state->regs[info.rs2]);
        state->pc += 4u;

        return ExecutionStatus::Success;
    }

    const char* GetName() const override {
        return Oper::name;
    }
};

struct AddOp {
    constexpr static const char* const name = "add";

    static uint32_t exec(uint32_t a, uint32_t b) {
        return a + b;
    }
};

struct SubOp {
    constexpr static const char* const name = "sub";

    static uint32_t exec(uint32_t a, uint32_t b) {
        return a - b; 
    };
};

struct SllOp {
    constexpr static const char* const name = "sll";

    static uint32_t exec(uint32_t a, uint32_t b) {
        return a << (b & 31);
    }
};

struct SrlOp {
    constexpr static const char* const name = "srl";

    static uint32_t exec(uint32_t a, uint32_t b) {
        return a >> (b & 31);
    }
};

struct SraOp {
    constexpr static const char* const name = "sra";

    static uint32_t exec(uint32_t a, uint32_t b) {
        return static_cast<uint32_t>( static_cast<int32_t>(a) >> (b & 31) );
    }
};
    
using Add = InstructionTypeR<AddOp>;
using Sub = InstructionTypeR<SubOp>;
using Sll = InstructionTypeR<SllOp>;
using Srl = InstructionTypeR<SrlOp>;
using Sra = InstructionTypeR<SraOp>;

} // namespace rv32i
} // namespace rvi