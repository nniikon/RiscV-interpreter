#pragma once

#include "rvi_instruction_interface.hpp"

#include <cstdint>
#include <memory>
#include <unordered_map>

namespace rvi {

class InstructionRegistry {
private:
    std::unordered_map<uint32_t, std::unique_ptr<IInstruction>> registry_;

public:
    InstructionRegistry();

    void RegisterInstruction(std::unique_ptr<IInstruction> instr);
    const IInstruction* GetInstruction(uint32_t extended_opcode) const;
};

} // namespace