#pragma once

#include <cassert>
#include <cstdint>
#include <cstring>
#include <memory>

#include "rvi_instruction_interface.hpp"
#include "rvi_instruction_registry.hpp"

namespace rvi {
namespace rv32i {

class Jalr : public IInstructionTypeI {
    ExecutionStatus Execute(InterpreterState* state) override {
        uint32_t addr = static_cast<uint32_t>(
            static_cast<int32_t>(state->regs[info_.rs1]) + info_.imm
        ) & ~1;
        uint32_t return_addr = state->pc + 4u;
        state->regs[info_.rd] = return_addr;

        state->pc = addr;

        return ExecutionStatus::Success;
    }

    const char* GetName()           const override { return "jalr"; };
    uint32_t    GetExtendedOpcode() const override { return 0x00; }
};

template <class Oper>
class Load : public IInstructionTypeI {
    ExecutionStatus Execute(InterpreterState* state) override {
        uint32_t addr = static_cast<uint32_t>(
            static_cast<int32_t>(state->regs[info_.rs1]) + info_.imm
        );

        auto value = state->memory.Get<typename Oper::type>(addr);

        if constexpr (Oper::need_sex) {
            int32_t sex_value = static_cast<int32_t>(value);
            state->regs[info_.rd] = static_cast<uint32_t>(sex_value);
        } else {
            state->regs[info_.rd] = static_cast<uint32_t>(value);
        }

        state->pc += 4u;

        return ExecutionStatus::Success;
    }

    const char* GetName()           const override { return Oper::name; }
    uint32_t    GetExtendedOpcode() const override { return Oper::extended_opcode; }
};

namespace {

struct LoadByteUnsignedOper {
    using type = uint8_t;
    static constexpr bool need_sex = false;
    static constexpr const char* name = "lbu";
    static constexpr uint32_t extended_opcode = 0x00;
};

struct LoadByteOper {
    using type = int8_t;
    static constexpr bool need_sex = true;
    static constexpr const char* name = "lb";
    static constexpr uint32_t extended_opcode = 0x00;
};

struct LoadHalfUnsignedOper {
    using type = uint16_t;
    static constexpr bool need_sex = false;
    static constexpr const char* name = "lhu";
    static constexpr uint32_t extended_opcode = 0x00;
};

struct LoadHalfOper {
    using type = int16_t;
    static constexpr bool need_sex = true;
    static constexpr const char* name = "lh";
    static constexpr uint32_t extended_opcode = 0x00;
};

struct LoadWordOper {
    using type = uint32_t;
    static constexpr bool need_sex = false;
    static constexpr const char* name = "lw";
    static constexpr uint32_t extended_opcode = 0x00;
};

} // anon namespace

using Lb  = Load<LoadByteOper>;
using Lbu = Load<LoadByteUnsignedOper>;
using Lh  = Load<LoadHalfOper>;
using Lhu = Load<LoadHalfUnsignedOper>;
using Lw  = Load<LoadWordOper>;

template <class Oper>
class Arithm : public IInstructionTypeI {
    ExecutionStatus Execute(InterpreterState* state) override {
        state->regs[info_.rd] = Oper::exec(state->regs[info_.rs1], info_.imm);
        state->pc += 4u;

        return ExecutionStatus::Success;
    }

    const char* GetName()           const override { return Oper::name; }
    uint32_t    GetExtendedOpcode() const override { return Oper::extended_opcode; }
};

namespace {

struct AddiOper {
    constexpr static const char* const name = "addi";

    static uint32_t exec(uint32_t a, uint32_t b) {
        return a + b;
    }

    static const uint32_t extended_opcode = 0x00;
};

struct StliOper {
    constexpr static const char* const name = "stli";

    static uint32_t exec(uint32_t a, uint32_t b) {
        auto a_i = static_cast<int32_t>(a);
        auto b_i = static_cast<int32_t>(b);
        return a_i < b_i ? 1 : 0;
    }

    static const uint32_t extended_opcode = 0x00;
};

struct StliuOper {
    constexpr static const char* const name = "stliu";

    static uint32_t exec(uint32_t a, uint32_t b) {
        return a < b ? 1 : 0;
    }

    static const uint32_t extended_opcode = 0x00;
};

struct XoriOper {
    constexpr static const char* const name = "xori";

    static uint32_t exec(uint32_t a, uint32_t b) {
        return a ^ b;
    }

    static const uint32_t extended_opcode = 0x00;
};

struct AndiOper {
    constexpr static const char* const name = "andi";

    static uint32_t exec(uint32_t a, uint32_t b) {
        return a & b;
    }

    static const uint32_t extended_opcode = 0x00;
};

struct OriOper {
    constexpr static const char* const name = "ori";

    static uint32_t exec(uint32_t a, uint32_t b) {
        return a | b;
    }

    static const uint32_t extended_opcode = 0x00;
};

struct SlliOper {
    constexpr static const char* const name = "slli";

    static uint32_t exec(uint32_t a, uint32_t b) {
        return a << (b & 0x1Fu);
    }

    static const uint32_t extended_opcode = 0x00;
};

struct SrliOper {
    constexpr static const char* const name = "srli";

    static uint32_t exec(uint32_t a, uint32_t b) {
        return a >> (b & 0x1Fu);
    }

    static const uint32_t extended_opcode = 0x00;
};

struct SraiOper {
    constexpr static const char* const name = "srai";

    static uint32_t exec(uint32_t a, uint32_t b) {
        return static_cast<uint32_t>(
            static_cast<int>(a) >> (b & 0x1Fu)
        );
    }

    static const uint32_t extended_opcode = 0x00;
};

} // anon namespace

using Srai  = Arithm<SraiOper>;
using Srli  = Arithm<SrliOper>;
using Slli  = Arithm<SlliOper>;
using Ori   = Arithm<OriOper>;
using Andi  = Arithm<AndiOper>;
using Xori  = Arithm<XoriOper>;
using Stliu = Arithm<StliuOper>;
using Stli  = Arithm<StliOper>;
using Addi  = Arithm<AddiOper>;

class Ecall : public IInstructionTypeI {
    ExecutionStatus Execute(InterpreterState* state) override {
        assert(0);
    }

    const char* GetName()           const override { return "Ecall"; }
    uint32_t    GetExtendedOpcode() const override { return 0x00; }
};

class Ebreak : public IInstructionTypeI {
    ExecutionStatus Execute(InterpreterState* state) override {
        assert(0);
    }

    const char* GetName()           const override { return "Ebreak"; }
    uint32_t    GetExtendedOpcode() const override { return 0x00; }
};

class Fence : public IInstructionTypeI {
    ExecutionStatus Execute(InterpreterState* state) override {
        assert(0);
    }

    const char* GetName()           const override { return "Fence"; }
    uint32_t    GetExtendedOpcode() const override { return 0x00; }
};

void RegisterInstructionsTypeI(rvi::InstructionRegistry* registry) {
    registry->RegisterInstruction(std::make_unique<Fence> ());
    registry->RegisterInstruction(std::make_unique<Ebreak>());
    registry->RegisterInstruction(std::make_unique<Ecall> ());
    registry->RegisterInstruction(std::make_unique<Ecall> ());
    registry->RegisterInstruction(std::make_unique<Srai>  ()); 
    registry->RegisterInstruction(std::make_unique<Srli>  ()); 
    registry->RegisterInstruction(std::make_unique<Slli>  ()); 
    registry->RegisterInstruction(std::make_unique<Ori>   ());  
    registry->RegisterInstruction(std::make_unique<Andi>  ()); 
    registry->RegisterInstruction(std::make_unique<Xori>  ()); 
    registry->RegisterInstruction(std::make_unique<Stliu> ());
    registry->RegisterInstruction(std::make_unique<Stli>  ()); 
    registry->RegisterInstruction(std::make_unique<Addi>  ());
    registry->RegisterInstruction(std::make_unique<Jalr>  ());
    registry->RegisterInstruction(std::make_unique<Lb>    ()); 
    registry->RegisterInstruction(std::make_unique<Lbu>   ());
    registry->RegisterInstruction(std::make_unique<Lh>    ()); 
    registry->RegisterInstruction(std::make_unique<Lhu>   ());
    registry->RegisterInstruction(std::make_unique<Lw>    ()); 
}

} // namespace rv32i
} // namespace rvi