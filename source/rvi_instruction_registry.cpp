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

bool InstructionRegistry::RegisterGroup(PerOpcodeGroup group,
                                        uint32_t opcode) {
    if (lookup_table_.at(opcode).IsInit()) {
        LOG_F(WARNING, "Opcode group is already registered");
        return false;
    }

    lookup_table_[opcode] = std::move(group);
    LOG_F(INFO, "Add group with opcode %x", opcode);

    return true;
}

namespace {


uint32_t key_r_type(rvi::InstructionDecodedCommonType info) {
    auto r = std::get<rvi::InstructionDecodedInfoTypeR>(info);

    return (r.funct3 << 7) | (r.funct7);
}

uint32_t key_i_arith(rvi::InstructionDecodedCommonType info) {
    auto i = std::get<rvi::InstructionDecodedInfoTypeI>(info);
    // Tell apart SRLI vs SRAI
    if (i.funct3 == 0b101u) {
        uint32_t shamt_hi = (static_cast<uint32_t>(i.imm) >> 5) & 0x1u; // 0 for SRLI, 1 for SRAI
        return static_cast<uint32_t>((i.funct3 << 1) | shamt_hi);
    }
    return static_cast<uint32_t>(i.funct3 << 1);
}

uint32_t key_i_load(rvi::InstructionDecodedCommonType info) {
    auto i = std::get<rvi::InstructionDecodedInfoTypeI>(info);
    return i.funct3;
}

uint32_t key_i_jalr(rvi::InstructionDecodedCommonType info) {
    auto i = std::get<rvi::InstructionDecodedInfoTypeI>(info);
    return i.funct3;
}

uint32_t key_i_fence(rvi::InstructionDecodedCommonType info) {
    auto i = std::get<rvi::InstructionDecodedInfoTypeI>(info);
    return i.funct3;
}

uint32_t key_i_system(rvi::InstructionDecodedCommonType info) {
    auto i = std::get<rvi::InstructionDecodedInfoTypeI>(info);
    // ECALL (imm=0) vs EBREAK (imm=1). Limit to 12-bit immediate range
    return static_cast<uint32_t>(static_cast<uint32_t>(i.imm) & 0xFFFu);
}

uint32_t key_s_store(rvi::InstructionDecodedCommonType info) {
    auto s = std::get<rvi::InstructionDecodedInfoTypeS>(info);
    return s.funct3 & 0x7u;
}

uint32_t key_b_branch(rvi::InstructionDecodedCommonType info) {
    auto b = std::get<rvi::InstructionDecodedInfoTypeB>(info);
    return b.funct3 & 0x7u;
}

uint32_t key_u_lui(rvi::InstructionDecodedCommonType /*info*/) {
    return 0u;
}

uint32_t key_u_auipc(rvi::InstructionDecodedCommonType /*info*/) {
    return 0u;
}

} // namespace

void rvi::RegisterOpcodeGroups(InstructionRegistry* registry) {
    registry->RegisterGroup(PerOpcodeGroup(/*size*/ 1024u, &key_r_type), 0x33u);

    registry->RegisterGroup(PerOpcodeGroup(/*size*/ 16u, &key_i_arith), 0x13u);

    registry->RegisterGroup(PerOpcodeGroup(/*size*/ 8u, &key_i_load), 0x03u);

    registry->RegisterGroup(PerOpcodeGroup(/*size*/ 8u, &key_s_store), 0x23u);

    registry->RegisterGroup(PerOpcodeGroup(/*size*/ 8u, &key_b_branch), 0x63u);

    registry->RegisterGroup(PerOpcodeGroup(/*size*/ 8u, &key_i_jalr), 0x67u);

    registry->RegisterGroup(PerOpcodeGroup(/*size*/ 1u, &key_u_lui), 0x37u);

    registry->RegisterGroup(PerOpcodeGroup(/*size*/ 1u, &key_u_auipc), 0x17u);

    registry->RegisterGroup(PerOpcodeGroup(/*size*/ 8u, &key_i_fence), 0x0Fu);

    registry->RegisterGroup(PerOpcodeGroup(/*size*/ 4096u, &key_i_system), 0x73u);
}
