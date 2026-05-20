# LeoFuzzer

Make code fail before users do.

LeoFuzzer is the Leopard-Crew probe and fuzzing bench.

It standardizes how Leopard-Crew bricks run probes, replay corpora, capture crashes, detect timeouts, and later perform simple mutation fuzzing.

## Core idea

LeoFuzzer is not a replacement for AFL, libFuzzer, Honggfuzz, or OSS-Fuzz.

It is a small, reproducible quality bench for Mac OS X 10.5.8 PowerPC and companion development systems.

## First target

V0.1 focuses on running external probe targets against corpus files.

Fuzzing comes after the runner, timeout handling, crash capture, and result reporting are reliable.

## Principle

Every brick must prove itself.
