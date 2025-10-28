#include "rvi_instruction_registry.hpp"
#include "rvi_read_binary.hpp"
#include <iomanip>
#include <iostream>

#include "rvi_instruction_type_b.hpp"
#include "rvi_instruction_type_i.hpp"
#include "rvi_instruction_type_r.hpp"
#include "rvi_instruction_type_s.hpp"
#include "rvi_instruction_type_u.hpp"

int main() {
    rvi::ReadBinary read_binary("tests/test");
    auto code_view = read_binary.GetTextSectionView();

    for (uint32_t i = 0; i < code_view.size() / 4; i++) {
        std::cout << std::hex << std::setfill('0') << std::setw(8) << reinterpret_cast<const uint32_t*>(code_view.data())[i] << "\n";
    }

    rvi::InstructionRegistry registry{};
    rvi::RegisterOpcodeGroups(&registry);

    rvi::rv32i::RegisterInstructionsTypeB(&registry);
    rvi::rv32i::RegisterInstructionsTypeI(&registry);
    rvi::rv32i::RegisterInstructionsTypeR(&registry);
    rvi::rv32i::RegisterInstructionsTypeS(&registry);
    rvi::rv32i::RegisterInstructionsTypeU(&registry);

    return 0;
}
