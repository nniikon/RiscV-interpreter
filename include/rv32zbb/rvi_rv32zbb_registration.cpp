#include "rvi_rv32zbb_registration.hpp"

#include "rv32zbb/rvi_rv32zbb_type_r.hpp"
#include "rv32zbb/rvi_rv32zbb_type_i.hpp"

using namespace rvi;

void rvi::rv32zbb::RegisterRV32zbb(InstructionRegistry* registry) {
    rv32zbb::RegisterInstructionsTypeR(registry);
    rv32zbb::RegisterInstructionsTypeI(registry);
}
