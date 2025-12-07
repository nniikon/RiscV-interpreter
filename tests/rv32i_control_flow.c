#include "test_io.h"

struct Input {
    int32_t for_limit;
    int32_t while_limit;
    int32_t while_skip;
    int32_t while_break;
    int32_t do_while_iters;
    int32_t case0;
    int32_t case2;
    int32_t case4;
    int32_t case_default;
};

struct Output {
    int32_t acc_for;
    int32_t acc_while;
    int32_t acc_do;
    int32_t acc_switch;
};

int main(void)
{
    struct Input in;

    if (!read_exact(&in, (long)sizeof(in))) {
        return 1;
    }

    int32_t acc_for = 0;
    for (int32_t i = 0; i < in.for_limit; i++) {
        if (i & 1) {
            acc_for += i;
        } else {
            acc_for -= i;
        }
    }

    int32_t acc_while = 0;
    int32_t n = 0;
    while (n < in.while_limit) {
        n++;
        if (n == in.while_skip) {
            continue;
        }
        acc_while += n;
        if (n == in.while_break) {
            break;
        }
    }

    int32_t acc_do = 1;
    int32_t iter = 0;
    do {
        acc_do <<= 1;
        iter++;
    } while (iter < in.do_while_iters);

    int32_t acc_switch = 0;
    for (int i = 0; i < 5; i++) {
        switch (i) {
            case 0: acc_switch += in.case0; break;
            case 2: acc_switch += in.case2; break;
            case 4: acc_switch += in.case4; break;
            default: acc_switch += in.case_default; break;
        }
    }

    struct Output out = { acc_for, acc_while, acc_do, acc_switch };
    write_all(&out, (long)sizeof(out));
    return 0;
}
