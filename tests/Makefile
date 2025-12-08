AS           := riscv64-unknown-elf-as
CC           := riscv64-unknown-elf-gcc
ABI          := ilp32
MARCH_I      := rv32i
MARCH_M      := rv32im
MARCH_F      := rv32if
MARCH_ZBB    := rv32izbb
ASFLAGS      := -mabi=$(ABI) -march=$(MARCH_I)
CFLAGS_BASE  := -mabi=$(ABI) -nostartfiles -nostdlib -static -ffreestanding -nodefaultlibs
CFLAGS_I     := $(CFLAGS_BASE) -march=$(MARCH_I)
CFLAGS_M     := $(CFLAGS_BASE) -march=$(MARCH_M)
CFLAGS_F     := $(CFLAGS_BASE) -march=$(MARCH_F)
CFLAGS_ZBB   := $(CFLAGS_BASE) -march=$(MARCH_ZBB)

RV32I_TEST_SRCS := \
	test_add.c \
	read_write_test.c \
	rv32i_add_sub.c \
	rv32i_bitwise.c \
	rv32i_compare.c \
	rv32i_control_flow.c \
	rv32i_memory.c \
	rv32i_shift.c \
	test_echo

RV32M_TEST_SRCS := \
	test_div.c \
	test_mul.c \
	test_fib.c \
	test_quick_sort.c \
	test_sort.c \
	test_z_avl.c \
	rv32m_div_rem_signed.c \
	rv32m_div_rem_unsigned.c \
	rv32m_mul.c

RV32F_TEST_SRCS := \
	rv32f_arith.c \
	rv32f_convert.c \
	rv32f_sign_compare.c

RV32ZBB_TEST_SRCS := \
	rv32zbb.c

RV32I_TEST_BINS := $(RV32I_TEST_SRCS:.c=)
RV32M_TEST_BINS := $(RV32M_TEST_SRCS:.c=)
RV32F_TEST_BINS := $(RV32F_TEST_SRCS:.c=)
RV32ZBB_TEST_BINS := $(RV32ZBB_TEST_SRCS:.c=)
TEST_BINS       := $(RV32I_TEST_BINS) $(RV32M_TEST_BINS) $(RV32F_TEST_BINS) $(RV32ZBB_TEST_BINS)

.PHONY: all tests clean

all: tests

tests: $(TEST_BINS)

api.o: api.s
	$(AS) $(ASFLAGS) $< -o $@

$(RV32I_TEST_BINS): %: %.c api.o
	$(CC) $(CFLAGS_I) api.o $< -o $@

$(RV32M_TEST_BINS): %: %.c api.o
	$(CC) $(CFLAGS_M) api.o $< -o $@

$(RV32F_TEST_BINS): %: %.c api.o
	$(CC) $(CFLAGS_F) api.o $< -o $@

$(RV32ZBB_TEST_BINS): %: %.c api.o
	$(CC) $(CFLAGS_ZBB) api.o $< -o $@

clean:
	rm -f api.o $(TEST_BINS)
