#include "test_io.h"

struct Input {
    uint32_t a;
    uint32_t b;
};

struct Output {
    uint32_t and_res;
    uint32_t or_res;
    uint32_t xor_res;
    uint32_t not_res;
};

int main(void)
{
    struct Input in;

    if (!read_exact(&in, (long)sizeof(in))) {
        return 1;
    }

    struct Output out;
    out.and_res = in.a & in.b;
    out.or_res  = in.a | in.b;
    out.xor_res = in.a ^ in.b;
    out.not_res = ~in.a;

    write_all(&out, (long)sizeof(out));
    return 0;
}
