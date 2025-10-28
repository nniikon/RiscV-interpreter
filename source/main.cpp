#include "rvi_instruction_registry.hpp"
#include "rvi_read_binary.hpp"
#include <iomanip>
#include <iostream>

#include "rv32i/rvi_rv32i_registration.hpp"

int main() {
    rvi::ReadBinary read_binary("tests/test");
    auto code_view = read_binary.GetTextSectionView();

    for (uint32_t i = 0; i < code_view.size() / 4; i++) {
        std::cout << std::hex << std::setfill('0') << std::setw(8) << reinterpret_cast<const uint32_t*>(code_view.data())[i] << "\n";
    }

    rvi::InstructionRegistry registry{};

    rvi::rv32i::RegisterRV32I(&registry);

    return 0;
}
