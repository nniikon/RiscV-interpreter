#include "test_io.h"

#include <stdint.h>

struct Input {
    uint32_t logic_a;
    uint32_t logic_b;
    uint32_t signed_a;
    uint32_t signed_b;
    uint32_t unsigned_a;
    uint32_t unsigned_b;
    uint32_t rotate_value;
    uint32_t rotate_amount;
    uint32_t zext_value;
    uint32_t clz_value;
    uint32_t ctz_value;
    uint32_t cpop_value;
    uint32_t sextb_value;
    uint32_t sexth_value;
    uint32_t rori_value;
    uint32_t orc_value;
    uint32_t rev_value;
};

struct Output {
    uint32_t andn_res;
    uint32_t orn_res;
    uint32_t xnor_res;
    uint32_t min_res;
    uint32_t max_res;
    uint32_t minu_res;
    uint32_t maxu_res;
    uint32_t rol_res;
    uint32_t ror_res;
    uint32_t zext_h_res;
    uint32_t clz_res;
    uint32_t ctz_res;
    uint32_t cpop_res;
    uint32_t sext_b_res;
    uint32_t sext_h_res;
    uint32_t rori_res;
    uint32_t orc_b_res;
    uint32_t rev8_res;
};

#define DECLARE_BINARY_OP(name, asm_name)                         \
    static inline uint32_t name(uint32_t lhs, uint32_t rhs) {     \
        uint32_t result;                                          \
        __asm__ volatile(asm_name " %0, %1, %2"                   \
                         : "=r"(result)                           \
                         : "r"(lhs), "r"(rhs));                   \
        return result;                                            \
    }

DECLARE_BINARY_OP(instr_andn, "andn")
DECLARE_BINARY_OP(instr_orn,  "orn")
DECLARE_BINARY_OP(instr_xnor, "xnor")
DECLARE_BINARY_OP(instr_min,  "min")
DECLARE_BINARY_OP(instr_max,  "max")
DECLARE_BINARY_OP(instr_minu, "minu")
DECLARE_BINARY_OP(instr_maxu, "maxu")
DECLARE_BINARY_OP(instr_rol,  "rol")
DECLARE_BINARY_OP(instr_ror,  "ror")

#undef DECLARE_BINARY_OP

static inline uint32_t instr_zext_h(uint32_t value) {
    uint32_t result;
    __asm__ volatile("zext.h %0, %1"
                     : "=r"(result)
                     : "r"(value));
    return result;
}

static inline uint32_t instr_clz(uint32_t value) {
    uint32_t result;
    __asm__ volatile("clz %0, %1"
                     : "=r"(result)
                     : "r"(value));
    return result;
}

static inline uint32_t instr_ctz(uint32_t value) {
    uint32_t result;
    __asm__ volatile("ctz %0, %1"
                     : "=r"(result)
                     : "r"(value));
    return result;
}

static inline uint32_t instr_cpop(uint32_t value) {
    uint32_t result;
    __asm__ volatile("cpop %0, %1"
                     : "=r"(result)
                     : "r"(value));
    return result;
}

static inline uint32_t instr_sext_b(uint32_t value) {
    uint32_t result;
    __asm__ volatile("sext.b %0, %1"
                     : "=r"(result)
                     : "r"(value));
    return result;
}

static inline uint32_t instr_sext_h(uint32_t value) {
    uint32_t result;
    __asm__ volatile("sext.h %0, %1"
                     : "=r"(result)
                     : "r"(value));
    return result;
}

#define RORI_SHAMT 7u

static inline uint32_t instr_rori(uint32_t value) {
    uint32_t result;
    __asm__ volatile("rori %0, %1, %2"
                     : "=r"(result)
                     : "r"(value), "I"(RORI_SHAMT));
    return result;
}

static inline uint32_t instr_orc_b(uint32_t value) {
    uint32_t result;
    __asm__ volatile("orc.b %0, %1"
                     : "=r"(result)
                     : "r"(value));
    return result;
}

static inline uint32_t instr_rev8(uint32_t value) {
    uint32_t result;
    __asm__ volatile("rev8 %0, %1"
                     : "=r"(result)
                     : "r"(value));
    return result;
}

int main(void) {
    struct Input in;
    if (!read_exact(&in, (long)sizeof(in))) {
        return 1;
    }

    struct Output out;
    out.andn_res   = instr_andn(in.logic_a, in.logic_b);
    out.orn_res    = instr_orn(in.logic_a, in.logic_b);
    out.xnor_res   = instr_xnor(in.logic_a, in.logic_b);
    out.min_res    = instr_min(in.signed_a, in.signed_b);
    out.max_res    = instr_max(in.signed_a, in.signed_b);
    out.minu_res   = instr_minu(in.unsigned_a, in.unsigned_b);
    out.maxu_res   = instr_maxu(in.unsigned_a, in.unsigned_b);
    out.rol_res    = instr_rol(in.rotate_value, in.rotate_amount);
    out.ror_res    = instr_ror(in.rotate_value, in.rotate_amount);
    out.zext_h_res = instr_zext_h(in.zext_value);
    out.clz_res    = instr_clz(in.clz_value);
    out.ctz_res    = instr_ctz(in.ctz_value);
    out.cpop_res   = instr_cpop(in.cpop_value);
    out.sext_b_res = instr_sext_b(in.sextb_value);
    out.sext_h_res = instr_sext_h(in.sexth_value);
    out.rori_res   = instr_rori(in.rori_value);
    out.orc_b_res  = instr_orc_b(in.orc_value);
    out.rev8_res   = instr_rev8(in.rev_value);

    write_all(&out, (long)sizeof(out));
    return 0;
}

