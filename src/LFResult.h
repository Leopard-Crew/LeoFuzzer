#ifndef LF_RESULT_H
#define LF_RESULT_H

#include <stdio.h>

typedef enum LFResultKind {
    LF_RESULT_OK = 0,
    LF_RESULT_DOMAIN_REJECT = 1,
    LF_RESULT_USAGE = 2,
    LF_RESULT_EXIT_ERROR = 3,
    LF_RESULT_CRASH = 4,
    LF_RESULT_SIGNAL = 5,
    LF_RESULT_TIMEOUT = 6,
    LF_RESULT_EXEC_ERROR = 7
} LFResultKind;

typedef struct LFRunResult {
    LFResultKind kind;
    int exit_code;
    int signal_number;
    int timed_out;
    double elapsed_ms;
} LFRunResult;

const char *LFResultKindName(LFResultKind kind);

void LFPrintHumanResult(
    FILE *out,
    const char *target_path,
    const char *input_path,
    const LFRunResult *result
);

void LFPrintTsvHeader(FILE *out);

void LFPrintTsvResult(
    FILE *out,
    const char *target_path,
    const char *input_path,
    const LFRunResult *result
);

#endif
