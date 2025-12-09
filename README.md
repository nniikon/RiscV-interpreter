# RISC-V interpreter

A RISC-V interpreter that supports 32-bit I, M, F, and Zbb extensions.

## Build

Clone the repository:
```
git clone https://github.com/nniikon/RiscV-interpreter.git --recursive
cd RiscV-interpreter
```

Build the project:
```bash
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
```

### Run

To run an interpreter on a compiled binary:
```
./build/rvi </path/to/binary> [args]
```

## Tests

You may either use a Docker image with a cross-compiler preinstalled, or install the toolchain locally 

### Option 1: Docker

Pull and run a docker image:
```
docker pull nniikon/rvinterpreter:latest
docker run -it -v "$PWD":/apt/ nniikon/rvinterpreter:latest /bin/bash
```

### Option 2: Local installation

On Ubuntu:
```
apt install gcc-riscv64-unknown-elf cmake python3
```

### Run tests

After setting up the environment, run the tests:
```
cd tests
python3 run_tests.py
```

This may take up to a minute to complete.

> [!NOTE]
> You can test your RISC-V interpreter by setting the path to it via the RVI environment variable.
