#include "rvi_instruction_registry.hpp"

using namespace rvi;

InstructionRegistry::InstructionRegistry()
    : registry_()
{}

void InstructionRegistry::RegisterInstruction(std::unique_ptr<IInstruction> instr) {
    auto extended_opcode = instr->GetExtendedOpcode();
    auto [it, inserted] = registry_.emplace(extended_opcode, std::move(instr));
    if (!inserted) {
        throw std::runtime_error("Duplicate opcode registration");
    }
}

const IInstruction* InstructionRegistry::GetInstruction(uint32_t extended_opcode) const {
    auto it = registry_.find(extended_opcode);
    if (it == registry_.end()) {
        return nullptr;
    }
    return it->second.get();
}