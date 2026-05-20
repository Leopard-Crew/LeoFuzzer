# LeoFuzzer Scope Lock

LeoFuzzer is the Leopard-Crew probe and fuzzing bench.

It exists to standardize, accelerate, and reproduce quality checks for Leopard-Crew bricks on Mac OS X 10.5.8 PowerPC and companion systems.

## In Scope

- Running external probe targets
- Replaying known corpus files
- Capturing crashes, abnormal exits, and timeouts
- Measuring basic runtime behavior
- Writing reproducible result reports
- Providing simple mutation-based fuzzing after the probe runner is stable
- Supporting Leopard/PPC-specific validation, including endian-sensitive behavior

## Out of Scope

- Replacing AFL, libFuzzer, Honggfuzz, or OSS-Fuzz
- Requiring LLVM sanitizers
- Becoming a general security framework
- Becoming a GUI test suite
- Owning the build systems of individual bricks
- Hiding target-specific bugs behind framework complexity

## Principle

Every brick must prove itself.
