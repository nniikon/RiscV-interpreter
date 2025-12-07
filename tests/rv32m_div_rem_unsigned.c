#include "test_io.h"

static uint32_t rv32m_divu(uint32_t a, uint32_t b)
{
    uint32_t res;
    __asm__ volatile ("divu %0, %1, %2" : "=r"(res) : "r"(a), "r"(b));
    return res;
}

static uint32_t rv32m_remu(uint32_t a, uint32_t b)
{
    uint32_t res;
    __asm__ volatile ("remu %0, %1, %2" : "=r"(res) : "r"(a), "r"(b));
    return res;
}

struct Input {
    uint32_t dividend;
    uint32_t divisor;
};

struct Output {
    uint32_t quotient;
    uint32_t remainder;
};

int main(void)
{
    struct Input in;

    if (!read_exact(&in, (long)sizeof(in))) {
        return 1;
    }

    struct Output out;
    out.quotient  = rv32m_divu(in.dividend, in.divisor);
    out.remainder = rv32m_remu(in.dividend, in.divisor);

    write_all(&out, (long)sizeof(out));
    return 0;
}
