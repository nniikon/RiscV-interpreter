#include "test_io.h"

#include <stdint.h>

static float fcvt_s_w(int32_t value)
{
    float result;
    __asm__ volatile("fcvt.s.w %0, %1" : "=f"(result) : "r"(value));
    return result;
}

static float fcvt_s_wu(uint32_t value)
{
    float result;
    __asm__ volatile("fcvt.s.wu %0, %1" : "=f"(result) : "r"(value));
    return result;
}

static int32_t fcvt_w_s(float value)
{
    int32_t result;
    __asm__ volatile("fcvt.w.s %0, %1" : "=r"(result) : "f"(value));
    return result;
}

static uint32_t fcvt_wu_s(float value)
{
    uint32_t result;
    __asm__ volatile("fcvt.wu.s %0, %1" : "=r"(result) : "f"(value));
    return result;
}

struct Input {
    int32_t  signed_value;
    uint32_t unsigned_value;
    float    float_for_signed;
    float    float_for_unsigned;
};

struct Output {
    float   from_signed;
    float   from_unsigned;
    int32_t to_signed;
    uint32_t to_unsigned;
};

int main(void)
{
    struct Input in;
    if (!read_exact(&in, (long)sizeof(in))) {
        return 1;
    }

    struct Output out;
    out.from_signed   = fcvt_s_w(in.signed_value);
    out.from_unsigned = fcvt_s_wu(in.unsigned_value);
    out.to_signed     = fcvt_w_s(in.float_for_signed);
    out.to_unsigned   = fcvt_wu_s(in.float_for_unsigned);

    write_all(&out, (long)sizeof(out));
    return 0;
}
