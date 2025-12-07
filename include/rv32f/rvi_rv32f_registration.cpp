#include "rvi_rv32f_registration.hpp"
#include "rv32f/rvi_rv32f_type_f_load.hpp"

using namespace rvi;

void rvi::rv32f::RegisterRV32F(InstructionRegistry* registry) {
    rvi::rv32f::RegisterOpcodeGroupTypeI_Flw(registry);
}