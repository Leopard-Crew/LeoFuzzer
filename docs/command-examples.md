# LeoFuzzer Command Examples

## Build

    make clean
    make
    make selftest

## Single Input

    bin/leofuzz --target bin/echo-target --input corpus/samples/hello.txt

## Single Input as TSV

    bin/leofuzz --target bin/echo-target --input corpus/samples/hello.txt --tsv

## Corpus Run

    bin/leofuzz --target bin/echo-target --corpus corpus/samples

## Corpus Run as TSV

    bin/leofuzz --target bin/echo-target --corpus corpus/samples --tsv

## Corpus Run with Reports

    bin/leofuzz --target bin/echo-target --corpus corpus/samples --results results/selftest

## Crash Finding

    bin/leofuzz --target bin/crash-target --input corpus/samples/crash.txt --results results/selftest-crash

## Timeout Finding

    bin/leofuzz --target bin/timeout-target --input corpus/samples/timeout.txt --timeout 1 --results results/selftest-timeout

## Execution Error

    bin/leofuzz --target bin/does-not-exist --input corpus/samples/hello.txt --results results/selftest-exec-error

## Domain Reject

    bin/leofuzz --target bin/echo-target --input corpus/samples/does-not-exist.txt --results results/selftest-domain-reject

## Replay Finding

    bin/leofuzz --replay results/selftest-crash/findings/000001-CRASH.txt --results results/replay-crash

## Replay with Target Override

    bin/leofuzz --replay results/selftest-crash/findings/000001-CRASH.txt --target bin/echo-target --results results/replay-mismatch
