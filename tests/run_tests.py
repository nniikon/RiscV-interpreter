#!/usr/bin/env python3
import os
import sys
import subprocess
from pathlib import Path

GREEN = "\033[92m"
RED = "\033[91m"
RESET = "\033[0m"

def build_binary(binary: Path) -> bool:
    print(f"[BUILD] {binary.name}")
    result = subprocess.run(
        ["make", binary.name],
        stdout=sys.stdout,
        stderr=sys.stderr,
    )
    if result.returncode != 0:
        print(f"{RED}[FAIL]{RESET} Build: {binary.name} (skipping run)")
        return False
    return True


def run_test(rvi: str, binary: Path, logdir: Path) -> bool:
    print(f"[RUN] {binary.name}")

    result = subprocess.run(
        [rvi, str(binary)],
        stdout=sys.stdout,
        stderr=subprocess.PIPE,
        text=True
    )

    if result.returncode == 0:
        print(f"{GREEN}[OK]{RESET} {binary.name}")
        return True

    logdir.mkdir(exist_ok=True)
    logfile = logdir / f"{binary.name}.stderr.log"
    logfile.write_text(result.stderr)

    print(f"{RED}[FAIL]{RESET} {binary.name}")
    print(f"Log saved to: {logfile}")

    return False


def main() -> int:
    tests = sorted(Path(".").glob("test_*.c"))
    if not tests:
        print("No test_*.c files found", file=sys.stderr)
        return 1

    rvi = os.environ.get("RVI", "../build/rvi")
    logdir = Path("logs")

    successes = 0
    failures = 0

    for src in tests:
        binary = src.with_suffix("")

        if not (binary.exists() and os.access(binary, os.X_OK)):
            if not build_binary(binary):
                failures += 1
                print()
                continue

        if run_test(rvi, binary, logdir):
            successes += 1
        else:
            failures += 1

        print()

    print("==================================")
    print("Test summary:")
    print(f"  Successful: {successes}")
    print(f"  Failed:     {failures}")
    print("==================================")

    return 1 if failures else 0


if __name__ == "__main__":
    raise SystemExit(main())
