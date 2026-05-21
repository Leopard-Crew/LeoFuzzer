CC ?= gcc
CFLAGS ?= -Wall -Wextra -O2
CPPFLAGS ?=

BIN_DIR = bin

LEOFUZZ_SOURCES = \
	tools/leofuzz/main.c \
	src/LFRunner.c \
	src/LFResult.c \
	src/LFTime.c

all: $(BIN_DIR)/leofuzz probes

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(BIN_DIR)/leofuzz: $(LEOFUZZ_SOURCES) src/LFRunner.h src/LFResult.h src/LFTime.h | $(BIN_DIR)
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

probe-crash: all
	-$(BIN_DIR)/leofuzz --target $(BIN_DIR)/crash-target --input corpus/samples/crash.txt

probe-timeout: all
	-$(BIN_DIR)/leofuzz --target $(BIN_DIR)/timeout-target --input corpus/samples/timeout.txt --timeout 1

selftest: probe probe-tsv probe-crash probe-timeout

clean:
	rm -rf $(BIN_DIR)

.PHONY: all clean probes probe probe-tsv probe-crash probe-timeout selftest
