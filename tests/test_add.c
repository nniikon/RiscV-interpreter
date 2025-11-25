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

CREATE_TEST( add_u8u8, uint8_t, +, uint8_t, uint8_t)
CREATE_TEST( add_u16u16, uint16_t, +, uint16_t, uint16_t)
CREATE_TEST( add_u32u32, uint32_t, +, uint32_t, uint32_t)
CREATE_TEST( add_u64u64, uint64_t, +, uint64_t, uint64_t)

CREATE_TEST( add_s8s8, int8_t, +, int8_t, int8_t)
CREATE_TEST( add_s16s16, int16_t, +, int16_t, int16_t)
CREATE_TEST( add_s32s32, int32_t, +, int32_t, int32_t)
CREATE_TEST( add_s64s64, int64_t, +, int64_t, int64_t)

CREATE_TEST( add_u8s8, uint8_t, +, int8_t, uint8_t)
CREATE_TEST( add_u16s16, uint16_t, +, int16_t, uint16_t)
CREATE_TEST( add_u32s32, uint32_t, +, int32_t, uint32_t)
CREATE_TEST( add_u64s64, uint64_t, +, int64_t, uint64_t)

CREATE_TEST( add_s8u8, int8_t, +, uint8_t, int8_t)
CREATE_TEST( add_s16u16, int16_t, +, uint16_t, int16_t)
CREATE_TEST( add_s32u32, int32_t, +, uint32_t, int32_t)
CREATE_TEST( add_s64u64, int64_t, +, uint64_t, int64_t)

int main( int argc, char* argv[])
{
    int res = 
        add_u8u8( 43, 85, 128)   +
        add_u16u16( 672, 318, 990)   +
        add_u32u32( 2345, 8765, 11110)   +
        add_u64u64( 123456, 654321, 777777)   +

        add_s8s8( -15, 22, 7)   +
        add_s16s16( 1234, -567, 667)   +
        add_s32s32( -3456, 12345, 8889)   +
        add_s64s64( 2345678, -1234567, 1111111)   +

        add_u8s8( 150, -120, 30)   +
        add_u16s16( 1200, -650, 550)   +
        add_u32s32( 4321, -1234, 3087)   +
        add_u64s64( 9876543, -1234567, 8641976)   +

        add_s8u8( -78, 150, 72)   +
        add_s16u16( -1000, 2500, 1500)   +
        add_s32u32( -200000, 400000, 200000)   +
        add_s64u64( 3456789, 1234567, 4691356);

    return res;
}
