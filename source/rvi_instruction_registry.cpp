#include "rvi_instruction_registry.hpp"

#include <cassert>
#include <cstddef>

#define LOGURU_WITH_STREAMS 1
#include "loguru.hpp"

using namespace rvi;

PerOpcodeGroup::PerOpcodeGroup()
    : get_key_(nullptr),
      decode_instruction_(nullptr),
      lookup_table_() {
}

PerOpcodeGroup::PerOpcodeGroup(size_t size,
                               GetOpcodeGroupUniqueKeyFuncPtr get_key,
                               DecodeInstructionToCommonTypeFuncPtr decode_instruction)
    : get_key_(get_key),
      decode_instruction_(decode_instruction),
      lookup_table_(size) {
}

bool PerOpcodeGroup::AddInstruction(std::unique_ptr<IInstruction> instr) {
    auto key = get_key_(instr->GetDecodedInfo());

    auto& entry = lookup_table_.at(key);

    if (entry.get() != nullptr)
        return false;

    entry = std::move(instr);
    LOG_F(INFO, "+--- " "Add instruction %6s to opcode group with key %x",
          entry->GetName(),
          key);

    return true;
}

bool PerOpcodeGroup::IsInit() const {
    if (get_key_ == nullptr || decode_instruction_ == nullptr) {
        assert(lookup_table_.size() == 0);

        return false;
    }

    assert(lookup_table_.size() > 0);

    return true;
}

InstructionLookupResult PerOpcodeGroup::GetInstruction(uint32_t instr) const {
    assert(IsInit());
    auto info = decode_instruction_(instr);
    auto key = get_key_(info);

    auto& entry = lookup_table_.at(key);

    if (entry.get() == nullptr) {
        LOG_F(WARNING, "No instruction for key %x in opcode group", key);
        return {nullptr, info};
    }

    LOG_F(INFO, "Fetch instruction %s(%x) with opcode %x", entry->GetName(), instr, entry->GetOpcode());

    return {entry.get(), info};
}

InstructionRegistry::InstructionRegistry()
    : lookup_table_() {
}

bool InstructionRegistry::RegisterInstruction(std::unique_ptr<IInstruction> instr) {
    auto opcode = instr->GetOpcode();

    if (!lookup_table_.at(opcode).IsInit()) {
        LOG_F(WARNING, "Opcode group is not registered");
        return false;
    }

    return lookup_table_.at(opcode).AddInstruction(std::move(instr));
}

bool InstructionRegistry::RegisterGroup(PerOpcodeGroup group,
                                        uint32_t opcode) {
    if (lookup_table_.at(opcode).IsInit()) {
        LOG_F(WARNING, "Opcode group is already registered");
        return false;
    }

    lookup_table_[opcode] = std::move(group);
    LOG_F(INFO, "");
    LOG_F(INFO, "+ Add group with opcode %x", opcode);

    return true;
}

static uint8_t get_opcode(uint32_t instruction) {
    return (uint8_t)(instruction & 0x7F);
}

InstructionLookupResult InstructionRegistry::GetInstruction(uint32_t instr) const {
    auto opcode = get_opcode(instr);

    auto& per_opcode_group = lookup_table_[opcode];
    if (!per_opcode_group.IsInit()) {
        LOG_F(WARNING, "PerOpcodeGroup for instruction %x with opcode %x is not registered", instr, opcode);
        assert(0);

        return {nullptr, InstructionDecodedCommonType{}};
    }
    return per_opcode_group.GetInstruction(instr);
}
