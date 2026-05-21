# LeoFuzzer

Make code fail before users do.

LeoFuzzer is the Leopard-Crew probe and fuzzing bench.

It standardizes how Leopard-Crew bricks run probes, replay corpora, capture crashes, detect timeouts, write reports, preserve finding inputs, and replay findings on Mac OS X 10.5.8 PowerPC and companion development systems.

## Current Baseline

Current sealed baseline:

    v0.1.8-replay-expected-kind

Confirmed on:

    Mac OS X 10.5.8 PowerPC

## What LeoFuzzer Can Do

LeoFuzzer currently supports:

- running one external target against one input file
- running one external target against a flat corpus directory
- stable sorted corpus traversal
- quiet target output for machine-readable runs
- TSV output
- report directories with `summary.txt` and `runs.tsv`
- crash detection
- signal detection
- timeout detection
- execution error detection
- expected domain-level rejection handling
- finding artifact creation
- finding input copy preservation
- finding replay
- replay expected-kind mismatch detection
- selftests for clean runs, findings, domain rejects, exec errors, replay, and replay mismatch

## Non-Goals

LeoFuzzer is not:

- an AFL replacement
- a libFuzzer replacement
- an OSS-Fuzz clone
- a sanitizer framework
- a GUI test suite
- a build-system owner for individual bricks

It is a small, reproducible Leopard-Crew quality bench.

## Basic Usage

Run one target against one file:

    bin/leofuzz --target bin/echo-target --input corpus/samples/hello.txt

Run one target against a corpus:

    bin/leofuzz --target bin/echo-target --corpus corpus/samples

Write reports:

    bin/leofuzz --target bin/echo-target --corpus corpus/samples --results results/selftest

Replay a finding:

    bin/leofuzz --replay results/selftest-crash/findings/000001-CRASH.txt

Replay with explicit target override:

    bin/leofuzz --replay finding.txt --target bin/alternate-target

## Report Output

A report directory contains:

    summary.txt
    runs.tsv

If findings occur, it also contains:

    findings/

Finding artifacts contain:

    000001-KIND.txt
    000001-KIND.input

The `.txt` file stores finding metadata.

The `.input` file is a byte-for-byte copy of the input used to reproduce the finding.

## Build

On Leopard:

    make clean
    make
    make selftest

## Principle

Every brick must prove itself.
