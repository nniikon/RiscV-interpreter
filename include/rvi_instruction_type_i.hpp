#pragma once

#include <cassert>
#include <cstdint>
#include <memory>

#include "rvi_decode_info.hpp"
#include "rvi_instruction_interface.hpp"
#include "rvi_instruction_registry.hpp"

namespace rvi {
namespace rv32i {

class Jalr : public IInstruction {
    const uint32_t kOpcode = 0x67;
public:
    ExecutionStatus Execute(InterpreterState* state) override {
        auto info = std::get<InstructionDecodedInfoTypeI>(info_);

        uint32_t addr = static_cast<uint32_t>(
            static_cast<int32_t>(state->regs[info.rs1]) + info.imm
        ) & ~1;

        uint32_t return_addr = state->pc + 4u;
        state->regs[info.rd] = return_addr;

        state->pc = addr;

        return ExecutionStatus::Success;
    }

    const char* GetName()   const override { return "jalr"; };
    uint32_t    GetOpcode() const override { return kOpcode; }

    InstructionDecodedCommonType GetDecodedInfo() const override {
        InstructionDecodedInfoTypeI info = {
            .opcode = kOpcode,
            .funct3 = 0b000,
        };

        return info;
    }
};

template <class Oper>
class Load : public IInstruction {
private:
    const uint32_t kOpcode = 0x03;
public:
    ExecutionStatus Execute(InterpreterState* state) override {
        auto info = std::get<InstructionDecodedInfoTypeI>(info_);

        uint32_t addr = static_cast<uint32_t>(
            static_cast<int32_t>(state->regs[info.rs1]) + info.imm
        );

        auto value = state->memory.Get<typename Oper::type>(addr);

        if constexpr (Oper::need_sex) {
            int32_t sex_value = static_cast<int32_t>(value);
            state->regs[info.rd] = static_cast<uint32_t>(sex_value);
        } else {
            state->regs[info.rd] = static_cast<uint32_t>(value);
        }

        state->pc += 4u;

        return ExecutionStatus::Success;
    }

    const char* GetName()   const override { return Oper::name; }
    uint32_t    GetOpcode() const override { return kOpcode; }

    InstructionDecodedCommonType GetDecodedInfo() const override {
        InstructionDecodedInfoTypeI info = {
            .opcode = kOpcode,
            .funct3 = Oper::funct3,
        };

        return info;
    }
};

namespace {

struct LoadByteUnsignedOper {
    using type = uint8_t;
    static constexpr bool need_sex = false;
    static constexpr const char* name = "lbu";
    static constexpr uint32_t funct3 = 0b100;
};

struct LoadByteOper {
    using type = int8_t;
    static constexpr bool need_sex = true;
    static constexpr const char* name = "lb";
    static constexpr uint32_t funct3 = 0b000;
};

struct LoadHalfUnsignedOper {
    using type = uint16_t;
    static constexpr bool need_sex = false;
    static constexpr const char* name = "lhu";
    static constexpr uint32_t funct3 = 0b101;
};

struct LoadHalfOper {
    using type = int16_t;
    static constexpr bool need_sex = true;
    static constexpr const char* name = "lh";
    static constexpr uint32_t funct3 = 0b001;
};

struct LoadWordOper {
    using type = uint32_t;
    static constexpr bool need_sex = false;
    static constexpr const char* name = "lw";
    static constexpr uint32_t funct3 = 0b010;
};

} // namespace

using Lb  = Load<LoadByteOper>;
using Lbu = Load<LoadByteUnsignedOper>;
using Lh  = Load<LoadHalfOper>;
using Lhu = Load<LoadHalfUnsignedOper>;
using Lw  = Load<LoadWordOper>;

template <class Oper>
class Arithm final : public IInstruction {
    static constexpr uint32_t kOpcode = 0x13u;
public:
    ExecutionStatus Execute(InterpreterState* state) override {
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
};

struct StliOper {
    constexpr static const char* const name = "stli";

    static uint32_t exec(uint32_t a, uint32_t b) {
        auto a_i = static_cast<int32_t>(a);
        auto b_i = static_cast<int32_t>(b);
        return a_i < b_i ? 1 : 0;
    }

    static constexpr uint32_t funct3 = 0b010u;
};

struct StliuOper {
    constexpr static const char* const name = "stliu";

    static uint32_t exec(uint32_t a, uint32_t b) {
        return a < b ? 1 : 0;
    }

    static constexpr uint32_t funct3 = 0b011u;
};

struct XoriOper {
    constexpr static const char* const name = "xori";

    static uint32_t exec(uint32_t a, uint32_t b) {
        return a ^ b;
    }

    static constexpr uint32_t funct3 = 0b100u;
};

struct AndiOper {
    constexpr static const char* const name = "andi";

    static uint32_t exec(uint32_t a, uint32_t b) {
        return a & b;
    }

    static constexpr uint32_t funct3 = 0b111u;
};

struct OriOper {
    constexpr static const char* const name = "ori";

    static uint32_t exec(uint32_t a, uint32_t b) {
        return a | b;
    }

    static constexpr uint32_t funct3 = 0b110u;
};

struct SlliOper {
    constexpr static const char* const name = "slli";

    static uint32_t exec(uint32_t a, uint32_t b) {
        return a << (b & 0x1Fu);
    }

    static constexpr uint32_t funct3 = 0b001u;
    static constexpr uint32_t funct7 = 0b0000000u; // encoded in imm[11:5]
};

struct SrliOper {
    constexpr static const char* const name = "srli";

    static uint32_t exec(uint32_t a, uint32_t b) {
        return a >> (b & 0x1Fu);
    }

    static constexpr uint32_t funct3 = 0b101u;
    static constexpr uint32_t funct7 = 0b0000000u; // encoded in imm[11:5]
};

struct SraiOper {
    constexpr static const char* const name = "srai";

    static uint32_t exec(uint32_t a, uint32_t b) {
        return static_cast<uint32_t>(
            static_cast<int>(a) >> (b & 0x1Fu)
        );
    }

    static constexpr uint32_t funct3 = 0b101u;
    static constexpr uint32_t funct7 = 0b0100000u; // encoded in imm[11:5]
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

class Ecall : public IInstruction {
    static constexpr uint32_t kOpcode = 0x73u;
public:
    ExecutionStatus Execute(InterpreterState* /*state*/) override {
        assert(0);
        return ExecutionStatus::Success;
    }

    const char* GetName()   const override { return "Ecall"; }
    uint32_t    GetOpcode() const override { return kOpcode; }

    InstructionDecodedCommonType GetDecodedInfo() const override {
        InstructionDecodedInfoTypeI info = {
            .opcode = kOpcode,
            .funct3 = 0b000u,
        };
        return info;
    }
};

class Ebreak : public IInstruction {
    static constexpr uint32_t kOpcode = 0x73u;
public:
    ExecutionStatus Execute(InterpreterState* /*state*/) override {
        assert(0);
        return ExecutionStatus::Success;
    }

    const char* GetName()   const override { return "Ebreak"; }
    uint32_t    GetOpcode() const override { return kOpcode; }

    InstructionDecodedCommonType GetDecodedInfo() const override {
        InstructionDecodedInfoTypeI info = {
            .opcode = kOpcode,
            .funct3 = 0b000u,
        };
        return info;
    }
};

class Fence : public IInstruction {
    static constexpr uint32_t kOpcode = 0x0Fu;
public:
    ExecutionStatus Execute(InterpreterState* /*state*/) override {
        assert(0);
        return ExecutionStatus::Success;
    }

    const char* GetName()   const override { return "Fence"; }
    uint32_t    GetOpcode() const override { return kOpcode; }

    InstructionDecodedCommonType GetDecodedInfo() const override {
        InstructionDecodedInfoTypeI info = {
            .opcode = kOpcode,
            .funct3 = 0b000u,
        };
        return info;
    }
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
