CC ?= gcc
CFLAGS ?= -Wall -Wextra -O2
CPPFLAGS ?=

BIN_DIR = bin

LEOFUZZ_SOURCES = \
	tools/leofuzz/main.c \
	src/LFCorpus.c \
	src/LFReplay.c \
	src/LFReport.c \
	src/LFRunner.c \
	src/LFResult.c \
	src/LFTime.c

all: $(BIN_DIR)/leofuzz probes

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(BIN_DIR)/leofuzz: $(LEOFUZZ_SOURCES) src/LFCorpus.h src/LFReplay.h src/LFReport.h src/LFRunner.h src/LFResult.h src/LFTime.h | $(BIN_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -Isrc -o $@ $(LEOFUZZ_SOURCES)

probes: \
	$(BIN_DIR)/echo-target \
	$(BIN_DIR)/crash-target \
	$(BIN_DIR)/timeout-target

$(BIN_DIR)/echo-target: probes/echo-target/echo-target.c | $(BIN_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ probes/echo-target/echo-target.c

$(BIN_DIR)/crash-target: probes/crash-target/crash-target.c | $(BIN_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ probes/crash-target/crash-target.c

$(BIN_DIR)/timeout-target: probes/timeout-target/timeout-target.c | $(BIN_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ probes/timeout-target/timeout-target.c

probe: all
	$(BIN_DIR)/leofuzz --target $(BIN_DIR)/echo-target --input corpus/samples/hello.txt

probe-tsv: all
	$(BIN_DIR)/leofuzz --target $(BIN_DIR)/echo-target --input corpus/samples/hello.txt --tsv

probe-corpus: all
	$(BIN_DIR)/leofuzz --target $(BIN_DIR)/echo-target --corpus corpus/samples

probe-corpus-tsv: all
	$(BIN_DIR)/leofuzz --target $(BIN_DIR)/echo-target --corpus corpus/samples --tsv

probe-results: all
	rm -rf results/selftest
	$(BIN_DIR)/leofuzz --target $(BIN_DIR)/echo-target --corpus corpus/samples --results results/selftest
	test -f results/selftest/summary.txt
	test -f results/selftest/runs.tsv
	test ! -d results/selftest/findings
	grep 'runs=3' results/selftest/summary.txt
	grep 'ok=3' results/selftest/summary.txt
	grep 'rejected=0' results/selftest/summary.txt
	grep 'findings=0' results/selftest/summary.txt
	grep '^OK' results/selftest/runs.tsv
	awk -F '\t' 'NR > 1 { if (NF != 7) exit 1 }' results/selftest/runs.tsv

probe-results-crash: all
	rm -rf results/selftest-crash
	-$(BIN_DIR)/leofuzz --target $(BIN_DIR)/crash-target --input corpus/samples/crash.txt --results results/selftest-crash
	test -f results/selftest-crash/summary.txt
	test -f results/selftest-crash/runs.tsv
	test -f results/selftest-crash/findings/000001-CRASH.txt
	test -f results/selftest-crash/findings/000001-CRASH.input
	grep 'kind=CRASH' results/selftest-crash/findings/000001-CRASH.txt
	grep 'input_copy_status=OK' results/selftest-crash/findings/000001-CRASH.txt
	grep 'runs=1' results/selftest-crash/summary.txt
	grep 'ok=0' results/selftest-crash/summary.txt
	grep 'rejected=0' results/selftest-crash/summary.txt
	grep 'findings=1' results/selftest-crash/summary.txt
	grep '^CRASH' results/selftest-crash/runs.tsv
	awk -F '\t' 'NR > 1 { if (NF != 7) exit 1 }' results/selftest-crash/runs.tsv

probe-results-timeout: all
	rm -rf results/selftest-timeout
	-$(BIN_DIR)/leofuzz --target $(BIN_DIR)/timeout-target --input corpus/samples/timeout.txt --timeout 1 --results results/selftest-timeout
	test -f results/selftest-timeout/summary.txt
	test -f results/selftest-timeout/runs.tsv
	test -f results/selftest-timeout/findings/000001-TIMEOUT.txt
	test -f results/selftest-timeout/findings/000001-TIMEOUT.input
	grep 'kind=TIMEOUT' results/selftest-timeout/findings/000001-TIMEOUT.txt
	grep 'input_copy_status=OK' results/selftest-timeout/findings/000001-TIMEOUT.txt
	grep 'runs=1' results/selftest-timeout/summary.txt
	grep 'ok=0' results/selftest-timeout/summary.txt
	grep 'rejected=0' results/selftest-timeout/summary.txt
	grep 'findings=1' results/selftest-timeout/summary.txt
	grep '^TIMEOUT' results/selftest-timeout/runs.tsv
	awk -F '\t' 'NR > 1 { if (NF != 7) exit 1 }' results/selftest-timeout/runs.tsv

probe-results-exec-error: all
	rm -rf results/selftest-exec-error
	-$(BIN_DIR)/leofuzz --target bin/does-not-exist --input corpus/samples/hello.txt --results results/selftest-exec-error
	test -f results/selftest-exec-error/summary.txt
	test -f results/selftest-exec-error/runs.tsv
	test -f results/selftest-exec-error/findings/000001-EXEC_ERROR.txt
	test -f results/selftest-exec-error/findings/000001-EXEC_ERROR.input
	grep 'kind=EXEC_ERROR' results/selftest-exec-error/findings/000001-EXEC_ERROR.txt
	grep 'input_copy_status=OK' results/selftest-exec-error/findings/000001-EXEC_ERROR.txt
	grep 'runs=1' results/selftest-exec-error/summary.txt
	grep 'ok=0' results/selftest-exec-error/summary.txt
	grep 'rejected=0' results/selftest-exec-error/summary.txt
	grep 'findings=1' results/selftest-exec-error/summary.txt
	grep '^EXEC_ERROR' results/selftest-exec-error/runs.tsv
	awk -F '\t' 'NR > 1 { if (NF != 7) exit 1 }' results/selftest-exec-error/runs.tsv

probe-results-domain-reject: all
	rm -rf results/selftest-domain-reject
	$(BIN_DIR)/leofuzz --target $(BIN_DIR)/echo-target --input corpus/samples/does-not-exist.txt --results results/selftest-domain-reject
	test -f results/selftest-domain-reject/summary.txt
	test -f results/selftest-domain-reject/runs.tsv
	test ! -d results/selftest-domain-reject/findings
	grep 'runs=1' results/selftest-domain-reject/summary.txt
	grep 'ok=0' results/selftest-domain-reject/summary.txt
	grep 'rejected=1' results/selftest-domain-reject/summary.txt
	grep 'findings=0' results/selftest-domain-reject/summary.txt
	grep '^DOMAIN_REJECT' results/selftest-domain-reject/runs.tsv
	awk -F '\t' 'NR > 1 { if (NF != 7) exit 1 }' results/selftest-domain-reject/runs.tsv

probe-replay-crash: all
	rm -rf results/selftest-replay-source-crash results/selftest-replay-crash
	-$(BIN_DIR)/leofuzz --target $(BIN_DIR)/crash-target --input corpus/samples/crash.txt --results results/selftest-replay-source-crash
	test -f results/selftest-replay-source-crash/findings/000001-CRASH.txt
	-$(BIN_DIR)/leofuzz --replay results/selftest-replay-source-crash/findings/000001-CRASH.txt --results results/selftest-replay-crash
	test -f results/selftest-replay-crash/summary.txt
	test -f results/selftest-replay-crash/runs.tsv
	test -f results/selftest-replay-crash/findings/000001-CRASH.txt
	grep 'runs=1' results/selftest-replay-crash/summary.txt
	grep 'findings=1' results/selftest-replay-crash/summary.txt
	grep '^CRASH' results/selftest-replay-crash/runs.tsv
	grep 'kind=CRASH' results/selftest-replay-crash/findings/000001-CRASH.txt
	awk -F '\t' 'NR > 1 { if (NF != 7) exit 1 }' results/selftest-replay-crash/runs.tsv

probe-replay-timeout: all
	rm -rf results/selftest-replay-source-timeout results/selftest-replay-timeout
	-$(BIN_DIR)/leofuzz --target $(BIN_DIR)/timeout-target --input corpus/samples/timeout.txt --timeout 1 --results results/selftest-replay-source-timeout
	test -f results/selftest-replay-source-timeout/findings/000001-TIMEOUT.txt
	-$(BIN_DIR)/leofuzz --replay results/selftest-replay-source-timeout/findings/000001-TIMEOUT.txt --timeout 1 --results results/selftest-replay-timeout
	test -f results/selftest-replay-timeout/summary.txt
	test -f results/selftest-replay-timeout/runs.tsv
	test -f results/selftest-replay-timeout/findings/000001-TIMEOUT.txt
	grep 'runs=1' results/selftest-replay-timeout/summary.txt
	grep 'findings=1' results/selftest-replay-timeout/summary.txt
	grep '^TIMEOUT' results/selftest-replay-timeout/runs.tsv
	grep 'kind=TIMEOUT' results/selftest-replay-timeout/findings/000001-TIMEOUT.txt
	awk -F '\t' 'NR > 1 { if (NF != 7) exit 1 }' results/selftest-replay-timeout/runs.tsv

probe-results-missing-parent: all
	rm -rf results/selftest-missing-parent
	-$(BIN_DIR)/leofuzz --target $(BIN_DIR)/echo-target --input corpus/samples/hello.txt --results results/selftest-missing-parent/out
	test ! -d results/selftest-missing-parent

probe-crash: all
	-$(BIN_DIR)/leofuzz --target $(BIN_DIR)/crash-target --input corpus/samples/crash.txt

probe-timeout: all
	-$(BIN_DIR)/leofuzz --target $(BIN_DIR)/timeout-target --input corpus/samples/timeout.txt --timeout 1

selftest: \
	probe \
	probe-tsv \
	probe-corpus \
	probe-corpus-tsv \
	probe-results \
	probe-results-crash \
	probe-results-timeout \
	probe-results-exec-error \
	probe-results-domain-reject \
	probe-replay-crash \
	probe-replay-timeout \
	probe-results-missing-parent \
	probe-crash \
	probe-timeout

clean:
	rm -rf $(BIN_DIR)

.PHONY: all clean probes probe probe-tsv probe-corpus probe-corpus-tsv probe-results probe-results-crash probe-results-timeout probe-results-exec-error probe-results-domain-reject probe-replay-crash probe-replay-timeout probe-results-missing-parent probe-crash probe-timeout selftest
