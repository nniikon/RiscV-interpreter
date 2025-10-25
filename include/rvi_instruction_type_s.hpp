#pragma once

#include "rvi_decode_info.hpp"
#include "rvi_instruction_interface.hpp"
#include "rvi_instruction_registry.hpp"
#include <cassert>

namespace rvi {
namespace rv32i {

template <class Oper>
class Save : public rvi::IInstruction {
    static constexpr uint32_t kOpcode = 0x23u;
public:
    ExecutionStatus Execute(InterpreterState* state) override {
        auto info = std::get<InstructionDecodedInfoTypeS>(info_);
        uint32_t addr = static_cast<uint32_t>(
            static_cast<int32_t>(state->regs[info.rs1]) + info.imm
        );

        typename Oper::type value = static_cast<typename Oper::type>(state->regs[info.rs2]);
        state->memory.Set<typename Oper::type>(addr, value);

        state->pc += 4u;
        return ExecutionStatus::Success;
    }

    const char* GetName()   const override { return Oper::name; }
    uint32_t    GetOpcode() const override { return kOpcode; }

    InstructionDecodedCommonType GetDecodedInfo() const override {
        InstructionDecodedInfoTypeS info = {
            .opcode = kOpcode,
            .funct3 = Oper::funct3,
        };
        return info;
    }
};

struct SaveWord {
    using type = uint32_t;
    static constexpr const char* name = "sw";
    static constexpr uint32_t funct3 = 0b010u;
};

struct SaveHalf {
    using type = uint16_t;
    static constexpr const char* name = "sh";
    static constexpr uint32_t funct3 = 0b001u;
};

struct SaveByte {
    using type = uint8_t;
    static constexpr const char* name = "sb";
    static constexpr uint32_t funct3 = 0b000u;
};

using Sw = Save<SaveWord>;
using Sh = Save<SaveHalf>;
using Sb = Save<SaveByte>;

void RegisterInstructionsTypeS(rvi::InstructionRegistry* registry) {
    registry->RegisterInstruction(std::make_unique<Sw>());
    registry->RegisterInstruction(std::make_unique<Sh>());
    registry->RegisterInstruction(std::make_unique<Sb>());
}

} // namespace rv32i
} // namespace rvi
