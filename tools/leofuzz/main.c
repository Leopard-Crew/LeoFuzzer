#include "LFRunner.h"
#include "LFResult.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void LFPrintUsage(FILE *out)
{
    fprintf(out, "Usage:\n");
    fprintf(out, "  leofuzz --target TARGET --input FILE [--timeout SECONDS] [--tsv]\n");
}

static int LFIsFinding(LFResultKind kind)
{
    if (kind == LF_RESULT_OK) {
        return 0;
    }

    if (kind == LF_RESULT_DOMAIN_REJECT) {
        return 0;
    }

    return 1;
}

int main(int argc, char **argv)
{
    const char *target_path;
    const char *input_path;
    int timeout_seconds;
    int tsv_output;
    int i;
    LFRunResult result;

    target_path = 0;
    input_path = 0;
    timeout_seconds = 5;
    tsv_output = 0;

    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--target") == 0) {
            if ((i + 1) >= argc) {
                LFPrintUsage(stderr);
                return 2;
            }

            target_path = argv[++i];
            continue;
        }

        if (strcmp(argv[i], "--input") == 0) {
            if ((i + 1) >= argc) {
                LFPrintUsage(stderr);
                return 2;
            }

            input_path = argv[++i];
            continue;
        }

        if (strcmp(argv[i], "--timeout") == 0) {
            if ((i + 1) >= argc) {
                LFPrintUsage(stderr);
                return 2;
            }

            timeout_seconds = atoi(argv[++i]);
            continue;
        }

        if (strcmp(argv[i], "--tsv") == 0) {
            tsv_output = 1;
            continue;
        }

        if (strcmp(argv[i], "--help") == 0) {
            LFPrintUsage(stdout);
            return 0;
        }

        fprintf(stderr, "LEOFUZZ:ERROR unknown-option=%s\n", argv[i]);
        LFPrintUsage(stderr);
        return 2;
    }

    if (target_path == 0 || input_path == 0) {
        LFPrintUsage(stderr);
        return 2;
    }

    if (LFRunTarget(target_path, input_path, timeout_seconds, &result) != 0) {
        fprintf(stderr, "LEOFUZZ:ERROR failed-to-run-target\n");
        return 1;
    }

    if (tsv_output) {
        LFPrintTsvHeader(stdout);
        LFPrintTsvResult(stdout, target_path, input_path, &result);
    } else {
        LFPrintHumanResult(stdout, target_path, input_path, &result);
    }

    if (LFIsFinding(result.kind)) {
        return 1;
    }

    return 0;
}
