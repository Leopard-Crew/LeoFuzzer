#include "LFRunner.h"
#include "LFResult.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void LFPrintUsage(FILE *out)
{
    fprintf(out, "Usage:\n");
    fprintf(out, "  leofuzz --target TARGET --input FILE [--timeout SECONDS] [--tsv]\n");
    fprintf(out, "  leofuzz --target TARGET --input FILE [--target-output inherit|quiet]\n");
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

static int LFParseOutputMode(const char *text, LFOutputMode *output_mode)
{
    if (strcmp(text, "inherit") == 0) {
        *output_mode = LF_OUTPUT_INHERIT;
        return 0;
    }

    if (strcmp(text, "quiet") == 0) {
        *output_mode = LF_OUTPUT_QUIET;
        return 0;
    }

    return -1;
}

int main(int argc, char **argv)
{
    const char *target_path;
    const char *input_path;
    int timeout_seconds;
    int tsv_output;
    int output_mode_explicit;
    int i;
    LFOutputMode output_mode;
    LFRunResult result;

    target_path = 0;
    input_path = 0;
    timeout_seconds = 5;
    tsv_output = 0;
    output_mode = LF_OUTPUT_INHERIT;
    output_mode_explicit = 0;

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

        if (strcmp(argv[i], "--target-output") == 0) {
            if ((i + 1) >= argc) {
                LFPrintUsage(stderr);
                return 2;
            }

            if (LFParseOutputMode(argv[++i], &output_mode) != 0) {
                fprintf(stderr, "LEOFUZZ:ERROR invalid-target-output=%s\n", argv[i]);
                LFPrintUsage(stderr);
                return 2;
            }

            output_mode_explicit = 1;
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

    if (tsv_output && !output_mode_explicit) {
        output_mode = LF_OUTPUT_QUIET;
    }

    if (LFRunTarget(target_path, input_path, timeout_seconds, output_mode, &result) != 0) {
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
