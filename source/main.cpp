#include "rvi_instruction_interface.hpp"
#include "rvi_instruction_registry.hpp"
#include "rvi_read_binary.hpp"

#include "rv32i/rvi_rv32i_registration.hpp"

#include "loguru.hpp"

rvi::InstructionRegistry GetReadyRegistry() {
    rvi::InstructionRegistry registry{};

    rvi::rv32i::RegisterRV32I(&registry);
    return registry;
}

int main() {
    rvi::ReadBinary read_binary("tests/test");
    auto section_info = read_binary.GetTextSectionView();
    auto code = reinterpret_cast<const uint32_t*>(section_info.section.data());

    auto registry = GetReadyRegistry();

    {
        for (size_t i = 0; i < section_info.section.size(); i++) {
            auto [instr_interface, decoded_info] = registry.GetInstruction(code[i]);
            LOG_F(INFO, "[%x] %x - %s ", sizeof(uint32_t) * i, code[i], instr_interface->GetName());
        }
    }


    rvi::InterpreterState state{};
    state.pc = section_info.start_offset;

    rvi::ExecutionStatus status = rvi::ExecutionStatus::Success;
    while (status == rvi::ExecutionStatus::Success) {
        auto instr_raw = code[state.pc];

        LOG_F(INFO, "Decoding instruction with pc = %x", state.pc);
        auto [instr_interface, decoded_info] = registry.GetInstruction(instr_raw);

        status = instr_interface->Execute(&state, decoded_info);
    }

    return 0;
}
