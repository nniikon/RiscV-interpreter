#!/usr/bin/env python3
import json
import os
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Iterable, List, Tuple

BASE_DIR = Path(__file__).resolve().parent
CASE_DIR = BASE_DIR / "cases"
LOG_DIR = BASE_DIR / "logs"

GREEN = "\033[32m"
RED = "\033[31m"
RESET = "\033[0m"


@dataclass
class Case:
    name: str
    stdin: bytes
    stdout: bytes
    exit_code: int


@dataclass
class Spec:
    binary: str
    path: Path
    cases: List[Case]


def decode_hex(raw: str, *, context: str) -> bytes:
    cleaned = "".join(ch for ch in raw.strip() if not ch.isspace() and ch != "_")
    if cleaned.startswith("0x") or cleaned.startswith("0X"):
        cleaned = cleaned[2:]
    if cleaned == "":
        return b""
    if len(cleaned) % 2 != 0:
        raise ValueError(f"{context}: hex data has odd length")
    try:
        return bytes.fromhex(cleaned)
    except ValueError as exc:
        raise ValueError(f"{context}: {exc}") from exc


def load_specs() -> List[Spec]:
    specs: List[Spec] = []
    json_files = sorted(CASE_DIR.glob("*.json"))
    if not json_files:
        raise SystemExit("No JSON specs found in cases/")

    for json_path in json_files:
        try:
            data = json.loads(json_path.read_text())
        except json.JSONDecodeError as exc:
            raise SystemExit(f"Failed to parse {json_path}: {exc}")

        binary = data.get("binary")
        if not isinstance(binary, str) or not binary:
            raise SystemExit(f"{json_path}: missing or invalid 'binary' field")

        raw_cases = data.get("cases")
        if not isinstance(raw_cases, list) or not raw_cases:
            raise SystemExit(f"{json_path}: missing or invalid 'cases' list")

        cases: List[Case] = []
        for idx, case in enumerate(raw_cases):
            if not isinstance(case, dict):
                raise SystemExit(f"{json_path}: case #{idx} is not an object")
            name = case.get("name")
            if not isinstance(name, str) or not name:
                raise SystemExit(f"{json_path}: case #{idx} is missing a name")
            stdin_hex = case.get("stdin_hex", "")
            stdout_hex = case.get("stdout_hex", "")
            exit_code = int(case.get("exit_code", 0))
            stdin_bytes = decode_hex(
                str(stdin_hex), context=f"{json_path}::{name} stdin_hex"
            )
            stdout_bytes = decode_hex(
                str(stdout_hex), context=f"{json_path}::{name} stdout_hex"
            )
            cases.append(
                Case(
                    name=name,
                    stdin=stdin_bytes,
                    stdout=stdout_bytes,
                    exit_code=exit_code,
                )
            )

        specs.append(Spec(binary=binary, path=json_path, cases=cases))

    return specs


def build_binaries(binaries: Iterable[str]) -> None:
    targets = sorted(set(binaries))
    if not targets:
        raise SystemExit("No binaries requested for build")

    result = subprocess.run(["make", *targets], cwd=BASE_DIR)
    if result.returncode != 0:
        raise SystemExit(result.returncode)


def save_failure_logs(binary: str, case: str, stdout: bytes, stderr: bytes) -> None:
    logdir = LOG_DIR / binary
    logdir.mkdir(parents=True, exist_ok=True)
    (logdir / f"{case}.stdout").write_bytes(stdout)
    (logdir / f"{case}.stderr").write_bytes(stderr)


def run_case(
    rvi: Path, binary_path: Path, case: Case
) -> Tuple[bool, List[str], bytes, bytes, int]:
    proc = subprocess.run(
        [str(rvi), str(binary_path)],
        input=case.stdin,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        cwd=BASE_DIR,
    )

    issues: List[str] = []
    if proc.returncode != case.exit_code:
        issues.append(f"exit code {proc.returncode} != expected {case.exit_code}")
    if proc.stdout != case.stdout:
        issues.append(
            f"stdout mismatch (expected {case.stdout.hex()} got {proc.stdout.hex()})"
        )

    return (not issues), issues, proc.stdout, proc.stderr, proc.returncode


def main() -> int:
    specs = load_specs()
    build_binaries(spec.binary for spec in specs)

    rvi_path = Path(os.environ.get("RVI", "../build/rvi")).resolve()
    if not rvi_path.exists():
        print(f"RVI binary not found at {rvi_path}", file=sys.stderr)
        return 1

    total_cases = 0
    failures = 0

    for spec in specs:
        binary_path = BASE_DIR / spec.binary
        if not binary_path.exists():
            print(f"[BUILD MISSING] {spec.binary}", file=sys.stderr)
            failures += len(spec.cases)
            continue

        print(f"\n== {spec.binary} ({spec.path.name}) ==")
        for case in spec.cases:
            total_cases += 1
            ok, issues, stdout, stderr, exit_code = run_case(
                rvi_path, binary_path, case
            )
            if ok:
                print(f"[{GREEN}OK{RESET}]   {case.name}")
                continue

            failures += 1
            print(f"[{RED}FAIL{RESET}] {case.name}")
            for issue in issues:
                print(f"  - {issue}")
            save_failure_logs(spec.binary, case.name, stdout, stderr)

    print("\nSummary:")
    print(f"  Total cases: {total_cases}")
    print(f"  Failures:    {failures}")
    if failures:
        print(f"  Logs:        {LOG_DIR}")

    return 0 if failures == 0 else 1


if __name__ == "__main__":
    raise SystemExit(main())