#include "rvi_instruction_registry.hpp"

#include <cassert>
#include <cstddef>

#define LOGURU_WITH_STREAMS 1
#include "loguru.hpp"

using namespace rvi;

PerOpcodeGroup::PerOpcodeGroup()
    : get_key_(nullptr),
      lookup_table_() {
}

PerOpcodeGroup::PerOpcodeGroup(size_t size, GetOpcodeGroupUniqueKeyFuncPtr get_key)
    : get_key_(get_key),
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
    if (get_key_ == nullptr) {
        assert(lookup_table_.size() == 0);

        return false;
    }

    assert(lookup_table_.size() > 0);

    return true;
}

const IInstruction* PerOpcodeGroup::GetInstruction(uint32_t instr) const {
    auto key = get_key_(instr);
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

const IInstruction* InstructionRegistry::GetInstruction(uint32_t instr) const {
    auto opcode = get_opcode(instr);

    auto& per_opcode_group = lookup_table_[opcode];
    if (!per_opcode_group.IsInit()) {
        LOG_F(WARNING, "PerOpcodeGroup for instruction %x with opcode %x is not registered", instr, opcode);
        assert(0);

        return nullptr;
    }
    per_opcode_group.GetInstruction(instr).;
}