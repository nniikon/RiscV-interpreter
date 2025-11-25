// I-type SYSTEM (ecall, ebreak)
#pragma once

#include <cassert>
#include <cstdint>
#include <cstdio>
#include <memory>

#include "rvi_decode_info.hpp"
#include "rvi_instruction_interface.hpp"
#include "rvi_instruction_registry.hpp"
#include "rvi_state.hpp"

#define LOGURU_WITH_STREAMS 1
#include "loguru.hpp"

namespace rvi {
namespace rv32i {

enum class EcallArgs {
    Read = 63,
    Write = 64,
    Exit = 93,
};

class Ecall final : public IInstruction {

private:
    ExecutionStatus Read(InterpreterState* state) const {
        auto fd = state->regs.Get(10); // a0
        assert(fd == 0);           // Only stdin is supported

        auto str = static_cast<uint32_t>(state->regs.Get(11)); // a1
        auto len = static_cast<uint32_t>(state->regs.Get(12)); // a2

        uint32_t bytes_read = 0;
        while (bytes_read < len) {
            int c = fgetc(stdin);
            if (c == EOF)
                break;

            state->memory.Set<char>(str + bytes_read, static_cast<char>(c));
            ++bytes_read;
        }

        state->regs.Set(10, bytes_read);
        state->pc += 4u;

        return ExecutionStatus::Success;
    }

    ExecutionStatus Write(InterpreterState* state) const {
        auto fd = state->regs.Get(10); // a0
        assert(fd == 1);           // Only stdout is supported

        auto str = static_cast<uint32_t>(state->regs.Get(11)); // a1
        auto len = static_cast<uint32_t>(state->regs.Get(12)); // a2

        uint32_t bytes_written = 0;
        while (bytes_written < len) {
            auto value = state->memory.Get<char>(str + bytes_written);
            if (std::fputc(static_cast<unsigned char>(value), stdout) == EOF)
                break;
            ++bytes_written;
        }

        std::fflush(stdout);
        state->regs.Set(10, bytes_written);
        state->pc += 4u;

        return ExecutionStatus::Success;
    }

    ExecutionStatus Exit(InterpreterState* state) const {
        std::fflush(stdout);
        state->pc += 4u;
        state->return_code = static_cast<int32_t>(state->regs.Get(10)); // a0
        return ExecutionStatus::Exit;
    }

public:
    static constexpr uint32_t kOpcode = 0x73u;

    ExecutionStatus Execute(InterpreterState* state,
                            const InstructionDecodedCommonType& /* decoded_info */) const override {
        auto arg = static_cast<EcallArgs>(state->regs.Get(17)); // a7
        switch (arg) {
            case EcallArgs::Read:
                return Read(state);

            case EcallArgs::Write:
                return Write(state);

            case EcallArgs::Exit:
                return Exit(state);

            default:
                LOG_F(ERROR, "Ecall argument %u not implemented", static_cast<uint32_t>(arg));
                assert(0);
        }
        return ExecutionStatus::Success;
    }

    const char* GetName()   const override { return "Ecall"; }
    uint32_t    GetOpcode() const override { return kOpcode; }

    InstructionDecodedCommonType GetDecodedInfo() const override {
        InstructionDecodedInfoTypeI info = {
            .opcode = kOpcode,
            .funct3 = 0b000u,
            .imm    = 0,
        };
        return info;
    }
};

class Ebreak final : public IInstruction {
public:
    static constexpr uint32_t kOpcode = 0x73u;

    ExecutionStatus Execute(InterpreterState* /*state*/,
                            const InstructionDecodedCommonType& /*decoded_info*/) const override {
        assert(0);
        return ExecutionStatus::Success;
    }

    const char* GetName()   const override { return "Ebreak"; }
    uint32_t    GetOpcode() const override { return kOpcode; }

    InstructionDecodedCommonType GetDecodedInfo() const override {
        InstructionDecodedInfoTypeI info = {
            .opcode = kOpcode,
            .funct3 = 0b000u,
            .imm    = 1,
        };
        return info;
    }
};

namespace {

inline void RegisterInstructionsTypeI_System(rvi::InstructionRegistry* registry) {
    registry->RegisterInstruction(std::make_unique<Ebreak>());
    registry->RegisterInstruction(std::make_unique<Ecall> ());
}

inline uint32_t KeyTypeI_System(InstructionDecodedCommonType info) {
    auto i = std::get<InstructionDecodedInfoTypeI>(info);
    assert(i.imm == 0 || i.imm == 1);

    return static_cast<uint32_t>(i.imm);
}

} // namespace

inline void RegisterOpcodeGroupTypeI_System(rvi::InstructionRegistry* registry) {
    registry->RegisterGroup(
        rvi::PerOpcodeGroup(/*size*/ 2u, &KeyTypeI_System, &DecodeInstructionToCommonTypeI),
        Ecall::kOpcode);

    RegisterInstructionsTypeI_System(registry);
}

} // namespace rv32i
} // namespace rvi
