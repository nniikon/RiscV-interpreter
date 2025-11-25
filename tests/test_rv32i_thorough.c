#include <stdint.h>

extern long read( int fd, char* data, long maxlen);
extern long write( int fd, const char* data, long len);
extern __attribute__((noreturn)) void exit( long status);

static int test_add_sub(void)
{
    uint32_t u;
    int32_t  s;

    u = 0u + 0u;
    if (u != 0u) return 1;

    u = 1u + 2u;
    if (u != 3u) return 1;

    u = 0xffffffffu;
    u = u + 1u;
    if (u != 0u) return 1;

    u = 4000000000u - 1u;      /* 4e9 - 1 */
    if (u != 3999999999u) return 1;

    s = 10 - 3;
    if (s != 7) return 1;

    s = -5 + 12;
    if (s != 7) return 1;

    s = 1000000 - 1000001;
    if (s != -1) return 1;

    return 0;
}

static int test_bitwise(void)
{
    uint32_t a = 0x0f0f0f0fu;
    uint32_t b = 0x33333333u;

    if ((a & b) != 0x03030303u) return 1;
    if ((a | b) != 0x3f3f3f3fu) return 1;
    if ((a ^ b) != 0x3c3c3c3cu) return 1;

    uint32_t c = 0xffffffffu;
    if (~c != 0x00000000u) return 1;

    return 0;
}

static int test_shifts(void)
{
    uint32_t u = 1u;

    if ((u << 0) != 1u) return 1;
    if ((u << 3) != 8u) return 1;
    if ((u << 31) != 0x80000000u) return 1;

    if ((0x80000000u >> 31) != 1u) return 1;

    int32_t s = -1;
    if ((s >> 1) != -1) return 1;

    s = -4;
    if ((s >> 1) != -2) return 1;

    uint8_t u8 = 0x80u;
    if ((uint8_t)(u8 >> 1) != 0x40u) return 1;

    return 0;
}

static int test_comparisons(void)
{
    int32_t a = -10;
    int32_t b = 20;

    if (!(a < b)) return 1;
    if (!(b > a)) return 1;
    if (a == b) return 1;
    if (!(a != b)) return 1;

    uint32_t ua = 10u;
    uint32_t ub = 20u;

    if (!(ua < ub)) return 1;

    ua = 0xfffffffeu;
    ub = 0xffffffffu;
    if (!(ua < ub)) return 1;

    if (!((int32_t)0 > -1)) return 1;

    return 0;
}

static int8_t   s8arr[4];
static uint8_t  u8arr[4];
static int16_t  s16arr[4];
static uint16_t u16arr[4];
static int32_t  s32arr[4];
static uint32_t u32arr[4];

static int test_memory(void)
{
    int32_t i;
    int32_t sum = 0;

    for (i = 0; i < 4; i++) {
        s8arr[i]   = (int8_t)(-1 - i);
        u8arr[i]   = (uint8_t)(1u + (uint32_t)i);
        s16arr[i]  = (int16_t)(-100 - i);
        u16arr[i]  = (uint16_t)(100u + (uint32_t)i);
        s32arr[i]  = -1000 - i;
        u32arr[i]  = 1000u + (uint32_t)i;
    }

    for (i = 0; i < 4; i++) {
        sum += s8arr[i];
        sum += (int32_t)u8arr[i];
        sum += s16arr[i];
        sum += (int32_t)u16arr[i];
        sum += s32arr[i];
        sum += (int32_t)u32arr[i];
    }

    if (sum != 0) return 1;

    return 0;
}

static int test_control_flow(void)
{
    int32_t i;
    int32_t acc1 = 0;

    for (i = 0; i < 10; i++) {
        if (i & 1) {
            acc1 += i;
        } else {
            acc1 -= i;
        }
    }
    if (acc1 != 5) return 1;

    int32_t acc2 = 0;
    int32_t n    = 0;

    while (n < 10) {
        n++;
        if (n == 3) {
            continue;
        }
        acc2 += n;
        if (n == 6) {
            break;
        }
    }
    if (acc2 != 18) return 1;

    int32_t acc3 = 1;
    int32_t k    = 0;

    do {
        acc3 <<= 1;
        k++;
    } while (k < 4);
    if (acc3 != 16) return 1;

    int32_t sw = 0;
    for (i = 0; i < 5; i++) {
        switch (i) {
            case 0: sw += 10; break;
            case 2: sw += 20; break;
            case 4: sw += 40; break;
            default: sw += 1; break;
        }
    }
    if (sw != 72) return 1;

    return 0;
}

int main(int argc, char* argv[])
{
    int res = 0;

    res |= test_add_sub();
    res |= test_bitwise();
    res |= test_shifts();
    res |= test_comparisons();
    res |= test_memory();
    res |= test_control_flow();

    return res ? 1 : 0;
}
