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

    virtual ~IInstruction() = 0;
};

class IInstructionTypeR : public IInstruction {
protected:
    InstructionDecodedInfoTypeR info_;

public:
    uint32_t GetExtendedOpcode() const override;
};

class IInstructionTypeI : public IInstruction {
protected:
    InstructionDecodedInfoTypeI info_;

public:
    uint32_t GetExtendedOpcode() const override;
};

class IInstructionTypeS : public IInstruction {
protected:
    InstructionDecodedInfoTypeS info_;

public:
    uint32_t GetExtendedOpcode() const override;
};

class IInstructionTypeU : public IInstruction {
protected:
    InstructionDecodedInfoTypeU info_;

public:
    uint32_t GetExtendedOpcode() const override;
};

class IInstructionTypeB : public IInstruction {
protected:
    InstructionDecodedInfoTypeB info_;

public:
    uint32_t GetExtendedOpcode() const override;
};

} // namespace