# LeoFuzzer Probe Contract

A LeoFuzzer target is an executable that accepts one input file path.

## Invocation

    target input-file

## Exit Codes

- 0: input was processed or rejected safely
- 1: expected domain-level rejection
- 2: invalid target usage
- crash, signal, or timeout: LeoFuzzer finding

## Output

Targets may print diagnostic lines.

Preferred prefixes:

- LEOFUZZ:OK
- LEOFUZZ:REJECT
- LEOFUZZ:ERROR
- LEOFUZZ:INFO

LeoFuzzer must not depend on human-readable text for correctness.
Exit status and process behavior are authoritative.
