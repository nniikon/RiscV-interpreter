#include "rvi_instruction_registry.hpp"

#include <cassert>
#include <cstddef>

#define LOGURU_WITH_STREAMS 1
#include "loguru.hpp"

using namespace rvi;

PerOpcodeGroup::PerOpcodeGroup(size_t size, GetOpcodeGroupUniqueKeyFuncPtr get_key)
    : get_key_(get_key),
      lookup_table_(size) {
}

bool PerOpcodeGroup::AddInstruction(std::unique_ptr<IInstruction> instr) {
    auto key = get_key_(instr->GetDecodedInfo());

    if (lookup_table_.at(key).get() != nullptr)
        return false;

    lookup_table_.at(key) = std::move(instr);
    LOG_F(INFO, "Add instruction %s to opcode group with key %x", instr->GetName(), key);

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

bool InstructionRegistry::RegisterGroup(const PerOpcodeGroup& group,
                                        uint32_t opcode) {
    if (lookup_table_.at(opcode).IsInit()) {
        LOG_F(WARNING, "Opcode group is already registered");
        return false;
    }

    lookup_table_[opcode] = std::move(group);
    LOG_F(INFO, "Add group with opcode %x", opcode);

    return true;
}