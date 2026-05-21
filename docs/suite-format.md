# LeoFuzzer Suite Format

LeoFuzzer V0.2 introduces plain text suite files.

A suite file describes one target run.

The format is deliberately small:

    key=value

Blank lines are ignored.

Lines beginning with `#` are comments.

## Supported Keys

    name=...
    target=...
    input=...
    corpus=...
    results=...
    timeout=...
    expect_runs=...
    expect_ok=...
    expect_rejected=...
    expect_findings=...

A suite must define exactly one of:

    input=...
    corpus=...

Replay suites are not supported yet.

## Example

    name=leoutf8-validate-valid
    target=build-work/leofuzz_validate_probe
    corpus=corpus/leofuzz/valid
    results=results/leofuzz/valid
    timeout=5
    expect_runs=5
    expect_ok=5
    expect_rejected=0
    expect_findings=0

Run:

    leofuzz --suite testsuites/leoutf8-validate-valid.suite

## Expectation Semantics

Suite expectations validate the actual run summary.

A mismatch prints:

    LEOFUZZ:SUITE_MISMATCH field=... expected=... actual=...

A suite mismatch causes a non-zero exit status.

Suite mismatch is not a target finding.

## Principle

Suites move brick-check knowledge out of Makefile bodies and into explicit, reusable test descriptions.
