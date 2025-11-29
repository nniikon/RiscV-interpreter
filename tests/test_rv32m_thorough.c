#include <stdint.h>

extern long read(int fd, char* data, long maxlen);
extern long write(int fd, const char* data, long len);
extern __attribute__((noreturn)) void exit(long status);

/* --- RV32M instruction wrappers (inline asm) ---------------------------- */

static uint32_t rv32m_mul(uint32_t a, uint32_t b)
{
    uint32_t res;
    __asm__ volatile ("mul %0, %1, %2"
                      : "=r"(res)
                      : "r"(a), "r"(b));
    return res;
}

static uint32_t rv32m_mulh(uint32_t a, uint32_t b)
{
    uint32_t res;
    __asm__ volatile ("mulh %0, %1, %2"
                      : "=r"(res)
                      : "r"(a), "r"(b));
    return res;
}

static uint32_t rv32m_mulhsu(uint32_t a, uint32_t b)
{
    uint32_t res;
    __asm__ volatile ("mulhsu %0, %1, %2"
                      : "=r"(res)
                      : "r"(a), "r"(b));
    return res;
}

static uint32_t rv32m_mulhu(uint32_t a, uint32_t b)
{
    uint32_t res;
    __asm__ volatile ("mulhu %0, %1, %2"
                      : "=r"(res)
                      : "r"(a), "r"(b));
    return res;
}

static uint32_t rv32m_div(uint32_t a, uint32_t b)
{
    uint32_t res;
    __asm__ volatile ("div %0, %1, %2"
                      : "=r"(res)
                      : "r"(a), "r"(b));
    return res;
}

static uint32_t rv32m_divu(uint32_t a, uint32_t b)
{
    uint32_t res;
    __asm__ volatile ("divu %0, %1, %2"
                      : "=r"(res)
                      : "r"(a), "r"(b));
    return res;
}

static uint32_t rv32m_rem(uint32_t a, uint32_t b)
{
    uint32_t res;
    __asm__ volatile ("rem %0, %1, %2"
                      : "=r"(res)
                      : "r"(a), "r"(b));
    return res;
}

static uint32_t rv32m_remu(uint32_t a, uint32_t b)
{
    uint32_t res;
    __asm__ volatile ("remu %0, %1, %2"
                      : "=r"(res)
                      : "r"(a), "r"(b));
    return res;
}

/* --- MUL, MULH, MULHSU, MULHU tests ------------------------------------ */

static int test_mul_family(void)
{
    struct {
        uint32_t a;
        uint32_t b;
        uint32_t lo;      /* mul  result (low 32 bits)          */
        uint32_t mulh;    /* mulh   result (signed * signed)    */
        uint32_t mulhu;   /* mulhu  result (unsigned * unsigned)*/
        uint32_t mulhsu;  /* mulhsu result (signed * unsigned)  */
    } cases[] = {
        /* a          b          mul(lo)    mulh       mulhu      mulhsu      */
        { 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U },
        { 0x00000001U, 0x00000002U, 0x00000002U, 0x00000000U, 0x00000000U, 0x00000000U },

        /* -1 * 1, -1 * -1 */
        { 0xffffffffU, 0x00000001U, 0xffffffffU, 0xffffffffU, 0x00000000U, 0xffffffffU },
        { 0xffffffffU, 0xffffffffU, 0x00000001U, 0x00000000U, 0xfffffffeU, 0xffffffffU },

        /* 12345 * 6789 and -12345 * 6789 */
        { 0x00003039U, 0x00001a85U, 0x04fed79dU, 0x00000000U, 0x00000000U, 0x00000000U },
        { 0xffffcfc7U, 0x00001a85U, 0xfb012863U, 0xffffffffU, 0x00001a84U, 0xffffffffU },

        /* edge around INT32_MAX / INT32_MIN */
        { 0x7fffffffU, 0x00000002U, 0xfffffffeU, 0x00000000U, 0x00000000U, 0x00000000U },
        { 0x80000000U, 0x00000002U, 0x00000000U, 0xffffffffU, 0x00000001U, 0xffffffffU },

        /* INT32_MAX * INT32_MAX, INT32_MIN * INT32_MIN */
        { 0x7fffffffU, 0x7fffffffU, 0x00000001U, 0x3fffffffU, 0x3fffffffU, 0x3fffffffU },
        { 0x80000000U, 0x80000000U, 0x00000000U, 0x40000000U, 0x40000000U, 0xc0000000U },
    };

    int i;
    for (i = 0; i < (int)(sizeof(cases) / sizeof(cases[0])); i++) {
        uint32_t a = cases[i].a;
        uint32_t b = cases[i].b;

        if (rv32m_mul(a, b)   != cases[i].lo)     return 1;
        if (rv32m_mulh(a, b)  != cases[i].mulh)   return 1;
        if (rv32m_mulhu(a, b) != cases[i].mulhu)  return 1;
        if (rv32m_mulhsu(a, b)!= cases[i].mulhsu) return 1;
    }

    return 0;
}

/* --- Signed DIV / REM tests (div, rem) --------------------------------- */

static int test_div_rem_signed(void)
{
    struct {
        uint32_t a;   /* dividend bits (interpreted as int32_t for spec) */
        uint32_t b;   /* divisor  bits (interpreted as int32_t)          */
        uint32_t q;   /* expected quotient (bit pattern)                 */
        uint32_t r;   /* expected remainder (bit pattern)                */
    } cases[] = {
        /* basic positives and negatives */
        { 0x00000007U, 0x00000002U, 0x00000003U, 0x00000001U }, /*  7 /  2 =  3, r=1  */
        { 0x00000007U, 0xfffffffeU, 0xfffffffdU, 0x00000001U }, /*  7 / -2 = -3, r=1  */
        { 0xfffffff9U, 0x00000002U, 0xfffffffdU, 0xffffffffU }, /* -7 /  2 = -3, r=-1 */
        { 0xfffffff9U, 0xfffffffeU, 0x00000003U, 0xffffffffU }, /* -7 / -2 =  3, r=-1 */

        /* cases where |dividend| < |divisor| */
        { 0x00000005U, 0x0000000aU, 0x00000000U, 0x00000005U }, /*  5 / 10 = 0, r= 5 */
        { 0xfffffffbU, 0x0000000aU, 0x00000000U, 0xfffffffbU }, /* -5 / 10 = 0, r=-5 */
        { 0x00000005U, 0xfffffff6U, 0x00000000U, 0x00000005U }, /*  5 / -10= 0, r= 5 */
        { 0xfffffffbU, 0xfffffff6U, 0x00000000U, 0xfffffffbU }, /* -5 / -10= 0, r=-5 */

        /* zero dividend */
        { 0x00000000U, 0x00000005U, 0x00000000U, 0x00000000U }, /* 0 / 5 = 0, r=0 */

        /* overflow case: INT32_MIN / -1 -> quotient=INT32_MIN, remainder=0 */
        { 0x80000000U, 0xffffffffU, 0x80000000U, 0x00000000U },

        /* INT32_MIN / 2, INT32_MAX / -1 */
        { 0x80000000U, 0x00000002U, 0xc0000000U, 0x00000000U }, /* -2147483648 / 2    */
        { 0x7fffffffU, 0xffffffffU, 0x80000001U, 0x00000000U }, /*  2147483647 / -1   */

        /* random-ish check: 123456789 / ±1 */
        { 0x075bcd15U, 0x00000001U, 0x075bcd15U, 0x00000000U }, /* 123456789 / 1      */
        { 0x075bcd15U, 0xffffffffU, 0xf8a432ebU, 0x00000000U }, /* 123456789 / -1     */

        /* division by zero cases: RISC-V semantics, no trap */
        { 0x00000001U, 0x00000000U, 0xffffffffU, 0x00000001U }, /*  1 / 0 -> q=-1, r=1 */
        { 0xffffffffU, 0x00000000U, 0xffffffffU, 0xffffffffU }, /* -1 / 0 -> q=-1, r=-1 */
        { 0x80000000U, 0x00000000U, 0xffffffffU, 0x80000000U }, /* INT32_MIN / 0       */
        { 0x00000000U, 0x00000000U, 0xffffffffU, 0x00000000U }, /* 0 / 0               */
    };

    int i;
    for (i = 0; i < (int)(sizeof(cases) / sizeof(cases[0])); i++) {
        uint32_t a = cases[i].a;
        uint32_t b = cases[i].b;

        if (rv32m_div(a, b) != cases[i].q) return 1;
        if (rv32m_rem(a, b) != cases[i].r) return 1;
    }

    return 0;
}

/* --- Unsigned DIVU / REMU tests ---------------------------------------- */

static int test_div_rem_unsigned(void)
{
    struct {
        uint32_t a;
        uint32_t b;
        uint32_t q;
        uint32_t r;
    } cases[] = {
        /* simple */
        { 0x00000000U, 0x00000001U, 0x00000000U, 0x00000000U },
        { 0x0000000aU, 0x00000003U, 0x00000003U, 0x00000001U },

        /* max values */
        { 0xffffffffU, 0x00000001U, 0xffffffffU, 0x00000000U },
        { 0xffffffffU, 0x00000002U, 0x7fffffffU, 0x00000001U },
        { 0xffffffffU, 0x80000000U, 0x00000001U, 0x7fffffffU },
        { 0x80000000U, 0x00000002U, 0x40000000U, 0x00000000U },

        /* zero dividend */
        { 0x00000000U, 0x00000005U, 0x00000000U, 0x00000000U },

        /* random-ish */
        { 0x12345678U, 0x00001111U, 0x00011112U, 0x00000246U },

        /* division by zero: DIVU -> q = 0xffffffff, REMU -> r = dividend */
        { 0x12345678U, 0x00000000U, 0xffffffffU, 0x12345678U },
        { 0x00000000U, 0x00000000U, 0xffffffffU, 0x00000000U },
    };

    int i;
    for (i = 0; i < (int)(sizeof(cases) / sizeof(cases[0])); i++) {
        uint32_t a = cases[i].a;
        uint32_t b = cases[i].b;

        if (rv32m_divu(a, b) != cases[i].q)  return 1;
        if (rv32m_remu(a, b) != cases[i].r)  return 1;
    }

    return 0;
}

/* --- Main --------------------------------------------------------------- */

int main(int argc, char* argv[])
{
    int res = 0;

    res |= test_mul_family();
    res |= test_div_rem_signed();
    res |= test_div_rem_unsigned();

    return res ? 1 : 0;
}
