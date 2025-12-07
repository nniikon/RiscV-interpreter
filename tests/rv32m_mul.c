#include "test_io.h"

static uint32_t rv32m_mul(uint32_t a, uint32_t b)
{
    uint32_t res;
    __asm__ volatile ("mul %0, %1, %2" : "=r"(res) : "r"(a), "r"(b));
    return res;
}

static uint32_t rv32m_mulh(uint32_t a, uint32_t b)
{
    uint32_t res;
    __asm__ volatile ("mulh %0, %1, %2" : "=r"(res) : "r"(a), "r"(b));
    return res;
}

static uint32_t rv32m_mulhsu(uint32_t a, uint32_t b)
{
    uint32_t res;
    __asm__ volatile ("mulhsu %0, %1, %2" : "=r"(res) : "r"(a), "r"(b));
    return res;
}

static uint32_t rv32m_mulhu(uint32_t a, uint32_t b)
{
    uint32_t res;
    __asm__ volatile ("mulhu %0, %1, %2" : "=r"(res) : "r"(a), "r"(b));
    return res;
}

struct Input {
    uint32_t a;
    uint32_t b;
};

struct Output {
    uint32_t mul;
    uint32_t mulh;
    uint32_t mulhu;
    uint32_t mulhsu;
};

int main(void)
{
    struct Input in;

    if (!read_exact(&in, (long)sizeof(in))) {
        return 1;
    }

    struct Output out;
    out.mul   = rv32m_mul(in.a, in.b);
    out.mulh  = rv32m_mulh(in.a, in.b);
    out.mulhu = rv32m_mulhu(in.a, in.b);
    out.mulhsu= rv32m_mulhsu(in.a, in.b);

    write_all(&out, (long)sizeof(out));
    return 0;
}
