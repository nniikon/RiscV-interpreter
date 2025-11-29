AS      := riscv64-unknown-elf-as
CC      := riscv64-unknown-elf-gcc
ABI     := ilp32
MARCH_I := rv32i
MARCH_M := rv32im
ASFLAGS := -mabi=$(ABI) -march=$(MARCH_I)
CFLAGS  := -mabi=$(ABI) -march=$(MARCH_I) -nostartfiles -nostdlib -static
CFLAGS_M := -mabi=$(ABI) -march=$(MARCH_M) -nostartfiles -nostdlib -static

RV32I_TEST_SRCS := test_add.c test_rv32i_thorough.c
RV32M_TEST_SRCS := test_rv32m_thorough.c
RV32I_TEST_BINS := $(RV32I_TEST_SRCS:.c=)
RV32M_TEST_BINS := $(RV32M_TEST_SRCS:.c=)

.PHONY: all tests run clean

all: tests

tests: $(RV32I_TEST_BINS) $(RV32M_TEST_BINS)

api.o: api.s
	$(AS) $(ASFLAGS) $< -o $@

$(RV32I_TEST_BINS): %: %.c api.o
	$(CC) $(CFLAGS) api.o $< -o $@

$(RV32M_TEST_BINS): %: %.c api.o
	$(CC) $(CFLAGS_M) api.o $< -o $@

run: tests
	./run_tests.sh

clean:
	rm -f api.o $(RV32I_TEST_BINS) $(RV32M_TEST_BINS)
