# LeoFuzzer Leopard Code Structure

LeoFuzzer is a Leopard-Crew command-line quality bench.

Its code structure follows the target system first: Mac OS X 10.5.8, Darwin, PowerPC, GCC 4.0 compatibility, and boring reproducibility.

## Core Rules

- Use plain C for the core.
- Keep GCC 4.0 on Leopard as the baseline compiler.
- Prefer Darwin/BSD/POSIX system calls where they are the native CLI path.
- Avoid mandatory CMake, Python, LLVM, sanitizer, or libFuzzer dependencies.
- Keep modules small and named by responsibility.
- Do not hide target-specific bugs behind framework complexity.
- Do not link LeoFuzzer into brick code; run external probe targets.
- Add CoreFoundation only when it provides real Leopard-native value, such as plist output.
- Treat PowerPC and big-endian behavior as first-class test concerns.

## Module Pattern

Source files use the `LF` prefix.

Expected modules:

- LFRunner: process execution, timeout, signal handling
- LFResult: result classification and formatting
- LFTime: portable Leopard-safe timing
- LFCorpus: corpus traversal
- LFReport: summary, TSV, and later plist output
- LFMutator: simple mutation fuzzing after the runner is stable
- LFBench: basic runtime measurement

## Output Policy

Human output is allowed for interactive runs.

Machine-readable output must be quiet and reproducible.

Preferred result formats:

- summary.txt
- runs.tsv
- later: results.plist via CoreFoundation

## Principle

Every brick must prove itself on the target system.
