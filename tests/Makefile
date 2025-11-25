AS      := riscv64-unknown-elf-as
CC      := riscv64-unknown-elf-gcc
ABI     := ilp32
MARCH   := rv32i
ASFLAGS := -mabi=$(ABI) -march=$(MARCH)
CFLAGS  := -mabi=$(ABI) -march=$(MARCH) -nostartfiles -nostdlib -static

TEST_SRCS := test_add.c test_rv32i_thorough.c
TEST_BINS := $(TEST_SRCS:.c=)

.PHONY: all tests run clean

all: tests

tests: $(TEST_BINS)

api.o: api.s
	$(AS) $(ASFLAGS) $< -o $@

%: %.c api.o
	$(CC) $(CFLAGS) api.o $< -o $@

run: tests
	./run_tests.sh

clean:
	rm -f api.o $(TEST_BINS)
