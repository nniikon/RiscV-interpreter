#include "rv32m/rvi_rv32m_registration.hpp"

#include "rv32m/rvi_rv32m_type_r.hpp"

using namespace rvi;

void rvi::rv32m::RegisterRV32M(InstructionRegistry* registry) {
    rv32m::RegisterInstructionsTypeR(registry);
}
