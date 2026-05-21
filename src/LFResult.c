#include "LFResult.h"

const char *LFResultKindName(LFResultKind kind)
{
    switch (kind) {
        case LF_RESULT_OK:
            return "OK";
        case LF_RESULT_DOMAIN_REJECT:
            return "DOMAIN_REJECT";
        case LF_RESULT_USAGE:
            return "USAGE";
        case LF_RESULT_EXIT_ERROR:
            return "EXIT_ERROR";
        case LF_RESULT_CRASH:
            return "CRASH";
        case LF_RESULT_SIGNAL:
            return "SIGNAL";
        case LF_RESULT_TIMEOUT:
            return "TIMEOUT";
        case LF_RESULT_EXEC_ERROR:
            return "EXEC_ERROR";
        default:
            return "UNKNOWN";
    }
}

void LFPrintHumanResult(
    FILE *out,
    const char *target_path,
    const char *input_path,
    const LFRunResult *result
)
{
    fprintf(out, "LEOFUZZ:RESULT kind=%s target=%s input=%s exit=%d signal=%d timeout=%d elapsed_ms=%.3f\n",
        LFResultKindName(result->kind),
        target_path,
        input_path,
        result->exit_code,
        result->signal_number,
        result->timed_out,
        result->elapsed_ms
    );
}

void LFPrintTsvHeader(FILE *out)
{
    fprintf(out, "kind\ttarget\tinput\texit_code\tsignal\ttimeout\telapsed_ms\n");
}

void LFPrintTsvResult(
    FILE *out,
    const char *target_path,
    const char *input_path,
    const LFRunResult *result
)
{
    fprintf(out, "%s\t%s\t%s\t%d\t%d\t%d\t%.3f\n",
        LFResultKindName(result->kind),
        target_path,
        input_path,
        result->exit_code,
        result->signal_number,
        result->timed_out,
        result->elapsed_ms
    );
}
