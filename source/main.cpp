#include "rvi_instruction_registry.hpp"
#include "rvi_read_binary.hpp"
#include <iomanip>
#include <iostream>

#include "rv32i/rvi_rv32i_registration.hpp"

rvi::InstructionRegistry GetReadyRegistry() {
    rvi::InstructionRegistry registry{};

    rvi::rv32i::RegisterRV32I(&registry);
    return registry;
}

int main() {
    rvi::ReadBinary read_binary("tests/test");
    auto code_view = read_binary.GetTextSectionView();

    auto registry = GetReadyRegistry();

    rvi::InterpreterState state{};

    for (uint32_t i = 0; i < code_view.size() / sizeof(uint32_t); i++) {
        auto instr = reinterpret_cast<const uint32_t*>(code_view.data())[i];
        auto [instruction, decoded_info] = registry.GetInstruction(instr);
        instruction->Execute(&state, decoded_info);
    }

    return 0;
}
