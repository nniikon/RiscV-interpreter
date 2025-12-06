#include <stdint.h>
extern long read( int fd, char* data, long maxlen);
extern long write( int fd, const char* data, long len);
extern __attribute__((noreturn)) void exit( long status);

#define CREATE_TEST( name, type0, operation, type1, type_res)   \
    int name( type0 op0, type1 op1, type_res res)               \
    {                                               \
        if ( ((op0) operation (op1)) != (res) )     \
        {                                           \
            return 1;                               \
        }                                           \
        return 0;                                   \
    }


CREATE_TEST( mul_u8u8, uint8_t, *, uint8_t, uint8_t)
CREATE_TEST( mul_u16u16, uint16_t, *, uint16_t, uint16_t)
CREATE_TEST( mul_u32u32, uint32_t, *, uint32_t, uint32_t)
CREATE_TEST( mul_u64u64, uint64_t, *, uint64_t, uint64_t)

CREATE_TEST( mul_s8s8, int8_t, *, int8_t, int8_t)
CREATE_TEST( mul_s16s16, int16_t, *, int16_t, int16_t)
CREATE_TEST( mul_s32s32, int32_t, *, int32_t, int32_t)
CREATE_TEST( mul_s64s64, int64_t, *, int64_t, int64_t)

CREATE_TEST( mul_u8s8, uint8_t, *, int8_t, int8_t)
CREATE_TEST( mul_u16s16, uint16_t, *, int16_t, int16_t)
CREATE_TEST( mul_u32s32, uint32_t, *, int32_t, int32_t)
CREATE_TEST( mul_u64s64, uint64_t, *, int64_t, int64_t)

CREATE_TEST( mul_s8u8, int8_t, *, uint8_t, int8_t)
CREATE_TEST( mul_s16u16, int16_t, *, uint16_t, int16_t)
CREATE_TEST( mul_s32u32, int32_t, *, uint32_t, int32_t)
CREATE_TEST( mul_s64u64, int64_t, *, uint64_t, int64_t)

int main( int argc, char* argv[])
{
    int res =
        mul_u8u8( 15, 8, 120)   +
        mul_u16u16( 45, 23, 1035)   +
        mul_u32u32( 234, 567, 132678)   +
        mul_u64u64( 1234, 5678, 7006652)   +

        mul_s8s8( -10, 12, -120)   +
        mul_s16s16( 45, -23, -1035)   +
        mul_s32s32( -234, 567, -132678)   +
        mul_s64s64( 1234, -5678, -7006652)   +

        mul_u8s8( 15, -8, -120)   +
        mul_u16s16( 45, -23, -1035)   +
        mul_u32s32( 234, -567, -132678)   +
        mul_u64s64( 1234, -5678, -7006652)   +

        mul_s8u8( -10, 12, -120)   +
        mul_s16u16( 45, 23, 1035)   +
        mul_s32u32( -234, -567, 132678)   +
        mul_s64u64( 1234, 5678, 7006652);

    return res;
}
