#include "test_io.h"

#include <stdint.h>

static float fsgnj_s(float mag, float sign)
{
    float result;
    __asm__ volatile("fsgnj.s %0, %1, %2" : "=f"(result) : "f"(mag), "f"(sign));
    return result;
}

static float fsgnjn_s(float mag, float sign)
{
    float result;
    __asm__ volatile("fsgnjn.s %0, %1, %2" : "=f"(result) : "f"(mag), "f"(sign));
    return result;
}

static float fsgnjx_s(float mag, float sign)
{
    float result;
    __asm__ volatile("fsgnjx.s %0, %1, %2" : "=f"(result) : "f"(mag), "f"(sign));
    return result;
}

static uint32_t feq_s(float lhs, float rhs)
{
    uint32_t result;
    __asm__ volatile("feq.s %0, %1, %2" : "=r"(result) : "f"(lhs), "f"(rhs));
    return result;
}

static uint32_t flt_s(float lhs, float rhs)
{
    uint32_t result;
    __asm__ volatile("flt.s %0, %1, %2" : "=r"(result) : "f"(lhs), "f"(rhs));
    return result;
}

static uint32_t fle_s(float lhs, float rhs)
{
    uint32_t result;
    __asm__ volatile("fle.s %0, %1, %2" : "=r"(result) : "f"(lhs), "f"(rhs));
    return result;
}

static uint32_t fclass_s(float value)
{
    uint32_t result;
    __asm__ volatile("fclass.s %0, %1" : "=r"(result) : "f"(value));
    return result;
}

struct Input {
    float sgn_mag;
    float sgn_sign;
    float cmp_lhs;
    float cmp_rhs;
    float class_normal;
    float class_zero;
    float class_special;
};

struct Output {
    float    sgnj;
    float    sgnjn;
    float    sgnjx;
    uint32_t feq;
    uint32_t flt;
    uint32_t fle;
    uint32_t class_normal;
    uint32_t class_zero;
    uint32_t class_special;
};

int main(void)
{
    struct Input in;
    if (!read_exact(&in, (long)sizeof(in))) {
        return 1;
    }

    struct Output out;
    out.sgnj          = fsgnj_s(in.sgn_mag, in.sgn_sign);
    out.sgnjn         = fsgnjn_s(in.sgn_mag, in.sgn_sign);
    out.sgnjx         = fsgnjx_s(in.sgn_mag, in.sgn_sign);
    out.feq           = feq_s(in.cmp_lhs, in.cmp_rhs);
    out.flt           = flt_s(in.cmp_lhs, in.cmp_rhs);
    out.fle           = fle_s(in.cmp_lhs, in.cmp_rhs);
    out.class_normal  = fclass_s(in.class_normal);
    out.class_zero    = fclass_s(in.class_zero);
    out.class_special = fclass_s(in.class_special);

    write_all(&out, (long)sizeof(out));
    return 0;
}
