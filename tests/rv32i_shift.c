#include "test_io.h"

struct Input {
    uint32_t left_value;
    uint32_t left_shamt;
    uint32_t right_value;
    uint32_t right_shamt;
    int32_t  signed_value;
    uint32_t byte_value;
    uint32_t byte_shamt;
};

struct Output {
    uint32_t left_result;
    uint32_t logical_right_result;
    int32_t  arithmetic_right_result;
    uint32_t byte_right_result;
};

int main(void)
{
    struct Input in;

    if (!read_exact(&in, (long)sizeof(in))) {
        return 1;
    }

    uint32_t left_amount  = in.left_shamt & 31u;
    uint32_t right_amount = in.right_shamt & 31u;
    uint32_t byte_amount  = in.byte_shamt & 7u;

    struct Output out;
    out.left_result          = in.left_value << left_amount;
    out.logical_right_result = in.right_value >> right_amount;
    out.arithmetic_right_result = in.signed_value >> right_amount;
    out.byte_right_result    = (uint32_t)(((uint8_t)in.byte_value) >> byte_amount);

    write_all(&out, (long)sizeof(out));
    return 0;
}
