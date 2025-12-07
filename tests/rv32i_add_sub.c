#include "test_io.h"

struct Input {
    uint32_t a;
    uint32_t b;
};

struct Output {
    uint32_t sum;
    uint32_t diff;
};

int main(void)
{
    struct Input in;

    if (!read_exact(&in, (long)sizeof(in))) {
        return 1;
    }

    struct Output out;
    out.sum  = in.a + in.b;
    out.diff = in.a - in.b;

    write_all(&out, (long)sizeof(out));
    return 0;
}
