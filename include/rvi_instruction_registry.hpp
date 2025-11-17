#pragma once

#include "rvi_config.hpp"
#include "rvi_decode_info.hpp"
#include "rvi_instruction_interface.hpp"

#include <array>
#include <cstdint>
#include <memory>
#include <vector>

namespace rvi {

using GetOpcodeGroupUniqueKeyFuncPtr = uint32_t (*)(InstructionDecodedCommonType);
class PerOpcodeGroup {
private:
    GetOpcodeGroupUniqueKeyFuncPtr get_key_;
    std::vector<std::unique_ptr<IInstruction>> lookup_table_;

public:
    PerOpcodeGroup();
    PerOpcodeGroup(size_t size, GetOpcodeGroupUniqueKeyFuncPtr get_key);
    bool IsInit() const;
    bool AddInstruction(std::unique_ptr<IInstruction> instr);
    const IInstruction* GetInstruction(uint32_t instr) const;
};
class InstructionRegistry {
private:
    std::array<PerOpcodeGroup, (1u << kOpcodeSize)> lookup_table_;

public:
    InstructionRegistry();

    bool RegisterInstruction(std::unique_ptr<IInstruction> instr);
    bool RegisterGroup(PerOpcodeGroup group, uint32_t opcode);
    const IInstruction* GetInstruction(uint32_t instr) const;
};

} // namespace
