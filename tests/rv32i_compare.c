#include "test_io.h"

struct Input {
    int32_t  a;
    int32_t  b;
    uint32_t ua;
    uint32_t ub;
};

struct Output {
    uint32_t signed_lt;
    uint32_t signed_gt;
    uint32_t signed_eq;
    uint32_t unsigned_lt;
    uint32_t unsigned_gt;
    uint32_t unsigned_eq;
};

int main(void)
{
    struct Input in;

    if (!read_exact(&in, (long)sizeof(in))) {
        return 1;
    }

    struct Output out;
    out.signed_lt   = (in.a < in.b);
    out.signed_gt   = (in.a > in.b);
    out.signed_eq   = (in.a == in.b);
    out.unsigned_lt = (in.ua < in.ub);
    out.unsigned_gt = (in.ua > in.ub);
    out.unsigned_eq = (in.ua == in.ub);

    write_all(&out, (long)sizeof(out));
    return 0;
}
