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
    virtual InstructionDecodedCommonType GetDecodedInfo() const = 0;
    virtual uint32_t                     GetOpcode() const = 0;
    virtual ExecutionStatus Execute(InterpreterState* state) = 0;

    virtual const char* GetName() const = 0;

    virtual ~IInstruction() = 0;

protected:
    InstructionDecodedCommonType info_;
};

} // namespace