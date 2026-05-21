#ifndef LF_RUNNER_H
#define LF_RUNNER_H

#include "LFResult.h"

typedef enum LFOutputMode {
    LF_OUTPUT_INHERIT = 0,
    LF_OUTPUT_QUIET = 1
} LFOutputMode;

int LFRunTarget(
    const char *target_path,
    const char *input_path,
    int timeout_seconds,
    LFOutputMode output_mode,
    LFRunResult *result
);

#endif
