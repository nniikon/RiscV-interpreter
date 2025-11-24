// Central registration for RV32I

#include "rv32i/rvi_rv32i_registration.hpp"

#include "rv32i/rvi_rv32i_type_b_branch.hpp"
#include "rv32i/rvi_rv32i_type_r.hpp"
#include "rv32i/rvi_rv32i_type_r_compare.hpp"
#include "rv32i/rvi_rv32i_type_s_store.hpp"
#include "rv32i/rvi_rv32i_type_u_lui.hpp"
#include "rv32i/rvi_rv32i_type_u_auipc.hpp"
#include "rv32i/rvi_rv32i_type_i_jalr.hpp"
#include "rv32i/rvi_rv32i_type_j_jal.hpp"
#include "rv32i/rvi_rv32i_type_i_load.hpp"
#include "rv32i/rvi_rv32i_type_i_arithm.hpp"
#include "rv32i/rvi_rv32i_type_i_fence.hpp"
#include "rv32i/rvi_rv32i_type_i_system.hpp"

using namespace rvi;

void rvi::rv32i::RegisterRV32I(InstructionRegistry* registry) {
    rv32i::RegisterOpcodeGroupTypeR_Arithm  (registry);
    rv32i::RegisterInstructionsTypeR_Compare(registry);
    rv32i::RegisterOpcodeGroupTypeI_Arithm  (registry);
    rv32i::RegisterOpcodeGroupTypeI_Load    (registry);
    rv32i::RegisterOpcodeGroupTypeS_Store   (registry);
    rv32i::RegisterOpcodeGroupTypeB_Branch  (registry);
    rv32i::RegisterOpcodeGroupTypeI_Jalr    (registry);
    rv32i::RegisterOpcodeGroupTypeJ_Jal     (registry);
    rv32i::RegisterOpcodeGroupTypeU_Lui     (registry);
    rv32i::RegisterOpcodeGroupTypeU_Auipc   (registry);
    rv32i::RegisterOpcodeGroupTypeI_Fence   (registry);
    rv32i::RegisterOpcodeGroupTypeI_System  (registry);
}
