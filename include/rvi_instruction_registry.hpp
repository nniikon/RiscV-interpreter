#pragma once

#include "rvi_config.hpp"
#include "rvi_decode_info.hpp"
#include "rvi_instruction_interface.hpp"

#include <array>
#include <cstdint>
#include <memory>
#include <vector>
#include <utility>

namespace rvi {

using GetOpcodeGroupUniqueKeyFuncPtr = uint32_t (*)(InstructionDecodedCommonType);
using DecodeInstructionToCommonTypeFuncPtr = InstructionDecodedCommonType (*)(uint32_t);
using InstructionLookupResult = std::pair<const IInstruction*, InstructionDecodedCommonType>;

class PerOpcodeGroup {
private:
    GetOpcodeGroupUniqueKeyFuncPtr get_key_;
    DecodeInstructionToCommonTypeFuncPtr decode_instruction_;
    std::vector<std::unique_ptr<IInstruction>> lookup_table_;

public:
    PerOpcodeGroup();
    PerOpcodeGroup(size_t size,
                   GetOpcodeGroupUniqueKeyFuncPtr get_key,
                   DecodeInstructionToCommonTypeFuncPtr decode_instruction);
    bool IsInit() const;
    bool AddInstruction(std::unique_ptr<IInstruction> instr);
    InstructionLookupResult GetInstruction(uint32_t instr) const;
};
class InstructionRegistry {
private:
    std::array<PerOpcodeGroup, (1u << kOpcodeSize)> lookup_table_;

public:
    InstructionRegistry();

    bool RegisterInstruction(std::unique_ptr<IInstruction> instr);
    bool RegisterGroup(PerOpcodeGroup group, uint32_t opcode);
    InstructionLookupResult GetInstruction(uint32_t instr) const;
};

} // namespace
