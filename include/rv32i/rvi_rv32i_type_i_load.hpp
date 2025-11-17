// I-type LOAD (lb, lbu, lh, lhu, lw)
#pragma once

#include <cstdint>
#include <memory>

#include "rvi_decode_info.hpp"
#include "rvi_instruction_interface.hpp"
#include "rvi_instruction_registry.hpp"

namespace rvi {
namespace rv32i {

template <class Oper>
class Load final : public IInstruction {
public:
    static constexpr uint32_t kOpcode = 0x03u;

    ExecutionStatus Execute(InterpreterState* state) const override {
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

namespace {

inline void RegisterInstructionsTypeI_Load(rvi::InstructionRegistry* registry) {
    registry->RegisterInstruction(std::make_unique<Lb>    ());
    registry->RegisterInstruction(std::make_unique<Lbu>   ());
    registry->RegisterInstruction(std::make_unique<Lh>    ());
    registry->RegisterInstruction(std::make_unique<Lhu>   ());
    registry->RegisterInstruction(std::make_unique<Lw>    ());
}

inline uint32_t KeyTypeI_Load(InstructionDecodedCommonType info) {
    auto i = std::get<InstructionDecodedInfoTypeI>(info);
    return i.funct3 & 0x7u;
}

} // namespace

inline void RegisterOpcodeGroupTypeI_Load(rvi::InstructionRegistry* registry) {
    registry->RegisterGroup(
        rvi::PerOpcodeGroup(/*size*/ 8u, &KeyTypeI_Load, &DecodeInstructionToCommonTypeI),
        Lb::kOpcode);

    RegisterInstructionsTypeI_Load(registry);
}

} // namespace rv32i
} // namespace rvi
