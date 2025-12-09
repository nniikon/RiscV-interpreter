#include "rvi_instruction_interface.hpp"
#include "rvi_instruction_registry.hpp"
#include "rvi_read_binary.hpp"

#include "rv32i/rvi_rv32i_registration.hpp"
#include "rv32m/rvi_rv32m_registration.hpp"
#include "rv32f/rvi_rv32f_registration.hpp"
#include "rv32zbb/rvi_rv32zbb_registration.hpp"

#include "loguru.hpp"
#include "cxxopts.hpp"
#include <iostream>

namespace {
rvi::InstructionRegistry GetReadyRegistry() {
    rvi::InstructionRegistry registry{};

    rvi::rv32i::RegisterRV32I(&registry);
    rvi::rv32m::RegisterRV32M(&registry);
    rvi::rv32f::RegisterRV32F(&registry);
    rvi::rv32zbb::RegisterRV32zbb(&registry);
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
    rvi::InterpreterState state{};

    uint32_t entry_point = 0;
    read_binary.LoadIntoMemory(&state.memory, &entry_point);

    auto registry = GetReadyRegistry();

    state.pc = entry_point;
    constexpr uint32_t kStackPadding = 0x10000u;
    const uint32_t stack_top = static_cast<uint32_t>(state.memory.Size() - kStackPadding) & ~0xFu;
    state.regs.Set(2u, stack_top); // x2 = sp

    rvi::ExecutionStatus status = rvi::ExecutionStatus::Success;
    while (status == rvi::ExecutionStatus::Success) {
        DLOG_F(INFO, "[pc = %x]", state.pc);
        auto instr_raw = state.memory.Read<uint32_t>(state.pc);

        auto [instr_interface, decoded_info] = registry.GetInstruction(instr_raw);

        status = instr_interface->Execute(&state, decoded_info);
    }

    DLOG_F(INFO, "Program exit with code %i", state.return_code);

    return state.return_code;
}
