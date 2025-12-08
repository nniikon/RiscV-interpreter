#include "test_io.h"

static float fadd_s(float lhs, float rhs)
{
    float result;
    __asm__ volatile("fadd.s %0, %1, %2" : "=f"(result) : "f"(lhs), "f"(rhs));
    return result;
}

static float fsub_s(float lhs, float rhs)
{
    float result;
    __asm__ volatile("fsub.s %0, %1, %2" : "=f"(result) : "f"(lhs), "f"(rhs));
    return result;
}

static float fmul_s(float lhs, float rhs)
{
    float result;
    __asm__ volatile("fmul.s %0, %1, %2" : "=f"(result) : "f"(lhs), "f"(rhs));
    return result;
}

static float fdiv_s(float lhs, float rhs)
{
    float result;
    __asm__ volatile("fdiv.s %0, %1, %2" : "=f"(result) : "f"(lhs), "f"(rhs));
    return result;
}

static float fsqrt_s(float value)
{
    float result;
    __asm__ volatile("fsqrt.s %0, %1" : "=f"(result) : "f"(value));
    return result;
}

static float fmin_s(float lhs, float rhs)
{
    float result;
    __asm__ volatile("fmin.s %0, %1, %2" : "=f"(result) : "f"(lhs), "f"(rhs));
    return result;
}

static float fmax_s(float lhs, float rhs)
{
    float result;
    __asm__ volatile("fmax.s %0, %1, %2" : "=f"(result) : "f"(lhs), "f"(rhs));
    return result;
}

struct Input {
    float lhs;
    float rhs;
};

struct Output {
    float add;
    float sub;
    float mul;
    float div;
    float sqrt;
    float fmin;
    float fmax;
};

int main(void)
{
    struct Input in;
    if (!read_exact(&in, (long)sizeof(in))) {
        return 1;
    }

    struct Output out;
    out.add  = fadd_s(in.lhs, in.rhs);
    out.sub  = fsub_s(in.lhs, in.rhs);
    out.mul  = fmul_s(in.lhs, in.rhs);
    out.div  = fdiv_s(in.lhs, in.rhs);
    out.sqrt = fsqrt_s(in.lhs);
    out.fmin = fmin_s(in.lhs, in.rhs);
    out.fmax = fmax_s(in.lhs, in.rhs);

    write_all(&out, (long)sizeof(out));
    return 0;
}
