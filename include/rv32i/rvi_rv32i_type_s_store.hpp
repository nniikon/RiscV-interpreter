// S-type STORE (sb, sh, sw)
#pragma once

#include <cstdint>
#include <memory>

#include "rvi_decode_info.hpp"
#include "rvi_instruction_interface.hpp"
#include "rvi_instruction_registry.hpp"

namespace rvi {
namespace rv32i {

template <class Oper>
class Save final : public rvi::IInstruction {
public:
    static constexpr uint32_t kOpcode = 0x23u;

    ExecutionStatus Execute(InterpreterState* state) const override {
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

namespace {

inline void RegisterInstructionsTypeS(rvi::InstructionRegistry* registry) {
    registry->RegisterInstruction(std::make_unique<Sw>());
    registry->RegisterInstruction(std::make_unique<Sh>());
    registry->RegisterInstruction(std::make_unique<Sb>());
}

inline uint32_t KeyTypeS_Store(InstructionDecodedCommonType info) {
    auto s = std::get<InstructionDecodedInfoTypeS>(info);
    return s.funct3 & 0x7u;
}

} // namespace

inline void RegisterOpcodeGroupTypeS_Store(rvi::InstructionRegistry* registry) {
    registry->RegisterGroup(rvi::PerOpcodeGroup(/*size*/ 8u, &KeyTypeS_Store), Sw::kOpcode);

    RegisterInstructionsTypeS(registry);
}

} // namespace rv32i
} // namespace rvi
