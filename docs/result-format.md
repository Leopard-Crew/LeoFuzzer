# LeoFuzzer Result Format

LeoFuzzer writes human-readable summaries first.

Preferred V0.1 outputs:

- summary.txt
- runs.tsv
- findings/

## Report Directory

The `--results DIR` option writes report files into one result directory.

V0.1 creates the result directory itself, but it does not create missing parent directories.

Example:

    leofuzz --target bin/echo-target --corpus corpus/samples --results results/selftest

Creates:

    results/selftest/summary.txt
    results/selftest/runs.tsv

If findings occur, LeoFuzzer also creates:

    results/selftest/findings/

If the result directory cannot be created or opened, LeoFuzzer fails before running the target.

## runs.tsv

`runs.tsv` contains one row per target invocation.

Columns:

    kind
    target
    input
    exit_code
    signal
    timeout
    elapsed_ms

All run rows must contain exactly seven tab-separated fields.

Expected `kind` values include:

- OK
- DOMAIN_REJECT
- USAGE
- EXIT_ERROR
- CRASH
- SIGNAL
- TIMEOUT
- EXEC_ERROR

## summary.txt

`summary.txt` contains one summary line:

    LEOFUZZ:SUMMARY target=... runs=... ok=... rejected=... findings=...

A finding is any run that is not `OK` and not `DOMAIN_REJECT`.

Expected findings include:

- crashes
- unexpected non-zero exits
- usage errors
- signals
- timeouts
- execution errors

## Finding Artifacts

For each finding, LeoFuzzer writes a metadata file and an input copy.

Example:

    findings/000001-CRASH.txt
    findings/000001-CRASH.input

The metadata file contains:

    LEOFUZZ:FINDING
    kind=...
    target=...
    input=...
    input_copy=...
    input_copy_status=...
    exit_code=...
    signal=...
    timeout=...
    elapsed_ms=...

The `.input` file is a byte-for-byte copy of the original input when possible.

Expected domain-level rejections do not create finding artifacts.

## Selftest Expectations

LeoFuzzer selftests verify reports for:

- clean OK corpus runs
- crash findings
- timeout findings
- execution errors
- expected domain-level rejections
- invalid result directory paths
- finding artifact creation
- non-finding runs not creating a findings directory

This is deliberate: LeoFuzzer must prove its own reporting contract before it is trusted as a brick quality bench.

## Target Output

Interactive runs may inherit target stdout and stderr.

Machine-readable runs must be quiet and reproducible.

When `--tsv`, `--corpus`, or `--results` is used, LeoFuzzer defaults to quiet target output unless explicitly overridden with:

    --target-output inherit

Supported target output modes:

- inherit: child stdout/stderr remain attached to the parent terminal
- quiet: child stdout/stderr are redirected to `/dev/null`

Later versions may add plist output for Leopard-native tooling.
