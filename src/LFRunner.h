#ifndef LF_RUNNER_H
#define LF_RUNNER_H

#include "LFResult.h"

int LFRunTarget(
    const char *target_path,
    const char *input_path,
    int timeout_seconds,
    LFRunResult *result
);

#endif
