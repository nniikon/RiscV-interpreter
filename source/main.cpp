#include "rvi_instruction_interface.hpp"
#include "rvi_instruction_registry.hpp"
#include "rvi_read_binary.hpp"

#include "rv32i/rvi_rv32i_registration.hpp"
#include "rv32m/rvi_rv32m_registration.hpp"

#include "loguru.hpp"
#include "cxxopts.hpp"
#include <iostream>

namespace {
rvi::InstructionRegistry GetReadyRegistry() {
    rvi::InstructionRegistry registry{};

    rvi::rv32i::RegisterRV32I(&registry);
    rvi::rv32m::RegisterRV32M(&registry);
    return registry;
}
} // namespace

int main(const int argc, const char* const* argv) {
    cxxopts::Options options("rvi", "RiscV Intepreter");
    options.add_options()
        ("input", "Executable elf file", cxxopts::value<std::string>())
        ("args", "Executable args", cxxopts::value<std::vector<std::string>>());

    options.parse_positional({"input", "args"});
    auto result = options.parse(argc, argv);

    if (!result.count("input")) {
        std::cout << options.help() << std::endl;
        return 1;
    }

    rvi::ReadBinary read_binary(result["input"].as<std::string>());
    auto section_info = read_binary.GetTextSectionView();
    auto code = reinterpret_cast<const uint32_t*>(section_info.section.data());

    auto registry = GetReadyRegistry();

    rvi::InterpreterState state{};
    state.pc = section_info.start_offset;

    rvi::ExecutionStatus status = rvi::ExecutionStatus::Success;
    while (status == rvi::ExecutionStatus::Success) {
        LOG_F(INFO, "[pc = %x]", state.pc);
        auto instr_raw = code[state.pc / 4];

        auto [instr_interface, decoded_info] = registry.GetInstruction(instr_raw);

        status = instr_interface->Execute(&state, decoded_info);
    }

    LOG_F(INFO, "Program exit with code %i", state.return_code);

    return state.return_code;
}
