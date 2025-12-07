#include "test_io.h"

struct Input {
    int32_t  s8_base;
    uint32_t u8_base;
    int32_t  s16_base;
    uint32_t u16_base;
    int32_t  s32_base;
    uint32_t u32_base;
};

struct Output {
    int32_t sum;
};

static int8_t   s8arr[4];
static uint8_t  u8arr[4];
static int16_t  s16arr[4];
static uint16_t u16arr[4];
static int32_t  s32arr[4];
static uint32_t u32arr[4];

int main(void)
{
    struct Input in;

    if (!read_exact(&in, (long)sizeof(in))) {
        return 1;
    }

    for (int i = 0; i < 4; i++) {
        s8arr[i]   = (int8_t)(in.s8_base - i);
        u8arr[i]   = (uint8_t)(in.u8_base + (uint32_t)i);
        s16arr[i]  = (int16_t)(in.s16_base - i);
        u16arr[i]  = (uint16_t)(in.u16_base + (uint32_t)i);
        s32arr[i]  = in.s32_base - i;
        u32arr[i]  = in.u32_base + (uint32_t)i;
    }

    int32_t sum = 0;
    for (int i = 0; i < 4; i++) {
        sum += s8arr[i];
        sum += (int32_t)u8arr[i];
        sum += s16arr[i];
        sum += (int32_t)u16arr[i];
        sum += s32arr[i];
        sum += (int32_t)u32arr[i];
    }

    struct Output out = { sum };
    write_all(&out, (long)sizeof(out));
    return 0;
}
