# LeoFuzzer External Brick Results

This document records external Leopard-Crew bricks that have been exercised through LeoFuzzer.

## LeoUTF8

Repository:

    quietcode-org/LeoUTF8

First integration baseline:

    v0.7.3-leofuzzer-validation-probe

Corrected semantic baseline:

    v0.7.3.1-leofuzzer-empty-input-fix

Integration documentation seal:

    v0.7.3.2-leofuzzer-integration-seal

## Probe

    Tests/leofuzz_validate_probe.c

## Corpus

    corpus/leofuzz/valid/
    corpus/leofuzz/invalid/

## Confirmed Platform

    Mac OS X 10.5.8 PowerPC

## Confirmed Command

    make leofuzz-check

## Confirmed Results

Valid corpus:

    runs=5
    ok=5
    rejected=0
    findings=0

Invalid corpus:

    runs=5
    ok=0
    rejected=5
    findings=0

## Notes

The first integration pass revealed an empty-input semantics issue in the probe.

The probe originally treated an empty file as a NULL input pointer with length 0.

The corrected probe now passes a non-NULL buffer with length 0, so an empty file is treated as valid zero-length UTF-8.

This is a useful first example of LeoFuzzer's role:

    The bench did not merely run tests.
    It exposed a boundary semantics issue in the probe contract.

## Status

LeoUTF8 is the first external brick confirmed through LeoFuzzer.

## LeoUTF8 Transform Probe Extension

Transform probe baseline:

    v0.7.4-leofuzzer-transform-probes

Transform documentation seal:

    v0.7.4.1-leofuzzer-transform-seal

Confirmed transform probes:

- NFC normalization
- NFD normalization
- Unicode case folding

Confirmed command:

    make leofuzz-transform-check

Confirmed results:

    NFC valid:        runs=5 ok=5 rejected=0 findings=0
    NFC invalid:      runs=5 ok=0 rejected=5 findings=0
    NFD valid:        runs=5 ok=5 rejected=0 findings=0
    NFD invalid:      runs=5 ok=0 rejected=5 findings=0
    CaseFold valid:   runs=5 ok=5 rejected=0 findings=0
    CaseFold invalid: runs=5 ok=0 rejected=5 findings=0

Status:

    LeoUTF8 is now covered by LeoFuzzer for validation and core UTF-8 transformations.
