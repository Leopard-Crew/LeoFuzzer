# LeoFuzzer Result Format

LeoFuzzer writes human-readable summaries first.

Preferred V0.1 outputs:

- summary.txt
- runs.tsv
- crashes/
- timeouts/

## Report Directory

The `--results DIR` option writes report files into one result directory.

V0.1 creates the result directory itself, but it does not create missing parent directories.

Example:

    leofuzz --target bin/echo-target --corpus corpus/samples --results results/selftest

Creates:

    results/selftest/summary.txt
    results/selftest/runs.tsv

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

## Target Output

Interactive runs may inherit target stdout and stderr.

Machine-readable runs must be quiet and reproducible.

When `--tsv`, `--corpus`, or `--results` is used, LeoFuzzer defaults to quiet target output unless explicitly overridden with:

    --target-output inherit

Supported target output modes:

- inherit: child stdout/stderr remain attached to the parent terminal
- quiet: child stdout/stderr are redirected to `/dev/null`

Later versions may add plist output for Leopard-native tooling.
