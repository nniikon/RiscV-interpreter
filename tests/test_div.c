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


CREATE_TEST( div_u8u8, uint8_t, /, uint8_t, uint8_t)
CREATE_TEST( div_u16u16, uint16_t, /, uint16_t, uint16_t)
CREATE_TEST( div_u32u32, uint32_t, /, uint32_t, uint32_t)
// CREATE_TEST( div_u64u64, uint64_t, /, uint64_t, uint64_t)

CREATE_TEST( div_s8s8, int8_t, /, int8_t, int8_t)
CREATE_TEST( div_s16s16, int16_t, /, int16_t, int16_t)
CREATE_TEST( div_s32s32, int32_t, /, int32_t, int32_t)
// CREATE_TEST( div_s64s64, int64_t, /, int64_t, int64_t)

CREATE_TEST( div_u8s8, uint8_t, /, int8_t, int8_t)
CREATE_TEST( div_u16s16, uint16_t, /, int16_t, int16_t)
CREATE_TEST( div_u32s32, uint32_t, /, int32_t, int32_t)
// CREATE_TEST( div_u64s64, uint64_t, /, int64_t, int64_t)

CREATE_TEST( div_s8u8, int8_t, /, uint8_t, int8_t)
CREATE_TEST( div_s16u16, int16_t, /, uint16_t, int16_t)
CREATE_TEST( div_s32u32, int32_t, /, uint32_t, int32_t)
// CREATE_TEST( div_s64u64, int64_t, /, uint64_t, int64_t)

int main( int argc, char* argv[])
{
    int res =
        div_u8u8( 120, 8, 15)   +
        div_u16u16( 1035, 23, 45)   +
        div_u32u32( 132678, 567, 234)   +
        // div_u64u64( 7006652, 5678, 1234)   +

        div_s8s8( -120, 12, -10)   +
        div_s16s16( -1035, 23, -45)   +
        div_s32s32( -132678, 567, -234)   +
        // div_s64s64( -7006652, 5678, -1234)   +

        div_u8s8( 120, 8, 15)   +
        div_u16s16( 1035, 23, 45)   +
        div_u32s32( 132678, 567, 234)   +
        // div_u64s64( 7006652, 5678, 1234)   +

        div_s8u8( -120, 12, -10)   +
        div_s16u16( 1035, 23, 45)   +
        div_s32u32( 132678, 567, 234)   +
        // div_s64u64( 7006652, 5678, 1234)   +
        0;

    return res;
}
