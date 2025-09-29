#pragma once

#include "rvi_decode_info.hpp"
#include "rvi_state.hpp"
#include <cstdint>

namespace rvi {

struct InterpreterState;

enum class ExecutionStatus {
    Success = 0,
};

class IInstruction {
public:
    virtual uint32_t GetExtendedOpcode() const = 0;
    virtual ExecutionStatus Execute(InterpreterState* state) = 0;

    virtual const char* GetName() const = 0;
};

class IInstructionTypeR : public IInstruction {
protected:
    InstructionDecodedInfoTypeR info;

public:
    IInstructionTypeR(uint32_t instr);

    uint32_t GetExtendedOpcode() const override;
};

} // namespace