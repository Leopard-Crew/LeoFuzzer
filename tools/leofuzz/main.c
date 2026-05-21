#include "LFCorpus.h"
#include "LFReport.h"
#include "LFRunner.h"
#include "LFResult.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct LFRunContext {
    const char *target_path;
    int timeout_seconds;
    int tsv_output;
    LFOutputMode output_mode;
    LFReport *report;
    unsigned long total_runs;
    unsigned long findings;
    unsigned long ok_runs;
    unsigned long rejected_runs;
} LFRunContext;

static void LFPrintUsage(FILE *out)
{
    fprintf(out, "Usage:\n");
    fprintf(out, "  leofuzz --target TARGET --input FILE [--timeout SECONDS] [--tsv]\n");
    fprintf(out, "  leofuzz --target TARGET --corpus DIR [--timeout SECONDS] [--tsv]\n");
    fprintf(out, "  leofuzz --target TARGET --input FILE [--target-output inherit|quiet]\n");
    fprintf(out, "  leofuzz --target TARGET --corpus DIR --results DIR\n");
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

static void LFUpdateRunCounts(LFRunContext *context, LFResultKind kind)
{
    context->total_runs++;

    if (kind == LF_RESULT_OK) {
        context->ok_runs++;
        return;
    }

    if (kind == LF_RESULT_DOMAIN_REJECT) {
        context->rejected_runs++;
        return;
    }

    context->findings++;
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

static int LFRunOneInput(const char *input_path, LFRunContext *context)
{
    LFRunResult result;

    if (LFRunTarget(
            context->target_path,
            input_path,
            context->timeout_seconds,
            context->output_mode,
            &result
        ) != 0) {
        fprintf(stderr, "LEOFUZZ:ERROR failed-to-run-target input=%s\n", input_path);
        context->findings++;
        context->total_runs++;
        return 0;
    }

    if (context->tsv_output) {
        LFPrintTsvResult(stdout, context->target_path, input_path, &result);
    } else {
        LFPrintHumanResult(stdout, context->target_path, input_path, &result);
    }

    if (context->report != 0) {
        if (LFReportWriteRun(context->report, context->target_path, input_path, &result) != 0) {
            fprintf(stderr, "LEOFUZZ:ERROR failed-to-write-run-report input=%s\n", input_path);
        }

        if (LFIsFinding(result.kind)) {
            if (LFReportWriteFindingArtifact(context->report, context->target_path, input_path, &result) != 0) {
                fprintf(stderr, "LEOFUZZ:ERROR failed-to-write-finding-artifact input=%s\n", input_path);
            }
        }
    }

    LFUpdateRunCounts(context, result.kind);

    return 0;
}

static int LFVisitOneCorpusFile(const char *file_path, void *user_data)
{
    LFRunContext *context;

    context = (LFRunContext *)user_data;

    return LFRunOneInput(file_path, context);
}

static void LFPrintSummary(const LFRunContext *context)
{
    fprintf(stdout, "LEOFUZZ:SUMMARY runs=%lu ok=%lu rejected=%lu findings=%lu\n",
        context->total_runs,
        context->ok_runs,
        context->rejected_runs,
        context->findings
    );
}

int main(int argc, char **argv)
{
    const char *target_path;
    const char *input_path;
    const char *corpus_path;
    const char *results_path;
    int timeout_seconds;
    int tsv_output;
    int output_mode_explicit;
    int i;
    LFOutputMode output_mode;
    LFRunContext context;
    LFReport report;
    LFReport *active_report;

    target_path = 0;
    input_path = 0;
    corpus_path = 0;
    results_path = 0;
    timeout_seconds = 5;
    tsv_output = 0;
    output_mode = LF_OUTPUT_INHERIT;
    output_mode_explicit = 0;
    active_report = 0;

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

        if (strcmp(argv[i], "--corpus") == 0) {
            if ((i + 1) >= argc) {
                LFPrintUsage(stderr);
                return 2;
            }

            corpus_path = argv[++i];
            continue;
        }

        if (strcmp(argv[i], "--results") == 0) {
            if ((i + 1) >= argc) {
                LFPrintUsage(stderr);
                return 2;
            }

            results_path = argv[++i];
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

    if (target_path == 0) {
        LFPrintUsage(stderr);
        return 2;
    }

    if ((input_path == 0 && corpus_path == 0) || (input_path != 0 && corpus_path != 0)) {
        fprintf(stderr, "LEOFUZZ:ERROR specify exactly one of --input or --corpus\n");
        LFPrintUsage(stderr);
        return 2;
    }

    if ((tsv_output || corpus_path != 0 || results_path != 0) && !output_mode_explicit) {
        output_mode = LF_OUTPUT_QUIET;
    }

    if (results_path != 0) {
        if (LFReportOpen(&report, results_path) != 0) {
            fprintf(stderr, "LEOFUZZ:ERROR failed-to-open-results path=%s\n", results_path);
            return 1;
        }

        active_report = &report;
    }

    context.target_path = target_path;
    context.timeout_seconds = timeout_seconds;
    context.tsv_output = tsv_output;
    context.output_mode = output_mode;
    context.report = active_report;
    context.total_runs = 0;
    context.findings = 0;
    context.ok_runs = 0;
    context.rejected_runs = 0;

    if (tsv_output) {
        LFPrintTsvHeader(stdout);
    }

    if (input_path != 0) {
        LFRunOneInput(input_path, &context);
    } else {
        if (LFVisitCorpusFiles(corpus_path, LFVisitOneCorpusFile, &context) != 0) {
            fprintf(stderr, "LEOFUZZ:ERROR failed-to-read-corpus path=%s\n", corpus_path);
            LFReportClose(active_report);
            return 1;
        }
    }

    if (!tsv_output && corpus_path != 0) {
        LFPrintSummary(&context);
    }

    if (active_report != 0) {
        if (LFReportWriteSummary(
                active_report,
                context.target_path,
                context.total_runs,
                context.ok_runs,
                context.rejected_runs,
                context.findings
            ) != 0) {
            fprintf(stderr, "LEOFUZZ:ERROR failed-to-write-summary-report\n");
            LFReportClose(active_report);
            return 1;
        }

        LFReportClose(active_report);
    }

    if (context.findings > 0) {
        return 1;
    }

    return 0;
}
