# LeoFuzzer V0.1.8 Release Seal

This document seals the LeoFuzzer V0.1.8 baseline.

## Baseline Tag

    v0.1.8-replay-expected-kind

## Confirmed Platform

Confirmed on:

    Mac OS X 10.5.8 PowerPC

## Capability Chain

V0.1.8 confirms the complete first LeoFuzzer quality chain:

    run target
    run corpus
    write reports
    classify results
    detect findings
    preserve finding input
    write finding metadata
    replay finding metadata
    rerun preserved input
    compare replay result kind with expected metadata kind
    detect replay mismatch

## Result Kinds

Current result kinds:

- OK
- DOMAIN_REJECT
- USAGE
- EXIT_ERROR
- CRASH
- SIGNAL
- TIMEOUT
- EXEC_ERROR

## Replay Semantics

A finding metadata file contains:

    kind=...
    target=...
    input_copy=...

Replay loads the metadata, runs `input_copy` against the recorded target, and compares the actual result kind with the expected `kind`.

If both match:

    mismatch=0

If they differ:

    LEOFUZZ:REPLAY_MISMATCH expected=... actual=...
    mismatch=1

A replay mismatch is a replay-contract failure, not a target finding.

Therefore:

- `runs.tsv` records the actual target result
- `summary.txt` records the actual target run summary
- the process exits non-zero if the replay contract is violated

## Selftest Coverage

The V0.1.8 selftest covers:

- one-file probe
- TSV output
- flat corpus traversal
- report writing
- OK report validation
- crash finding report validation
- timeout finding report validation
- execution error report validation
- expected domain rejection validation
- finding artifact creation
- finding input copy preservation
- replay of crash finding
- replay of timeout finding
- replay mismatch detection
- missing result parent directory handling

## Deliberate Limits

V0.1.8 does not yet include:

- mutation fuzzing
- corpus minimization
- recursive corpus traversal
- plist reports
- checksum metadata
- structured suite files

Those should come after the V0.1.8 quality bench remains stable.
