#include "LFReport.h"

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

static char *LFReportJoinPath(const char *left, const char *right)
{
    size_t left_length;
    size_t right_length;
    int needs_slash;
    char *path;

    if (left == 0 || right == 0) {
        return 0;
    }

    left_length = strlen(left);
    right_length = strlen(right);
    needs_slash = 1;

    if (left_length > 0 && left[left_length - 1] == '/') {
        needs_slash = 0;
    }

    path = (char *)malloc(left_length + right_length + (needs_slash ? 2 : 1));

    if (path == 0) {
        return 0;
    }

    memcpy(path, left, left_length);

    if (needs_slash) {
        path[left_length] = '/';
        memcpy(path + left_length + 1, right, right_length + 1);
    } else {
        memcpy(path + left_length, right, right_length + 1);
    }

    return path;
}

static int LFReportEnsureDirectory(const char *path)
{
    struct stat st;

    if (path == 0) {
        return -1;
    }

    if (mkdir(path, 0775) == 0) {
        return 0;
    }

    if (errno != EEXIST) {
        return -1;
    }

    if (stat(path, &st) != 0) {
        return -1;
    }

    if (!S_ISDIR(st.st_mode)) {
        return -1;
    }

    return 0;
}

int LFReportOpen(LFReport *report, const char *results_path)
{
    char *summary_path;
    char *runs_path;

    if (report == 0 || results_path == 0) {
        return -1;
    }

    report->summary_file = 0;
    report->runs_tsv_file = 0;

    if (LFReportEnsureDirectory(results_path) != 0) {
        return -1;
    }

    summary_path = LFReportJoinPath(results_path, "summary.txt");

    if (summary_path == 0) {
        return -1;
    }

    runs_path = LFReportJoinPath(results_path, "runs.tsv");

    if (runs_path == 0) {
        free(summary_path);
        return -1;
    }

    report->summary_file = fopen(summary_path, "w");
    report->runs_tsv_file = fopen(runs_path, "w");

    free(summary_path);
    free(runs_path);

    if (report->summary_file == 0 || report->runs_tsv_file == 0) {
        LFReportClose(report);
        return -1;
    }

    LFPrintTsvHeader(report->runs_tsv_file);

    return 0;
}

void LFReportClose(LFReport *report)
{
    if (report == 0) {
        return;
    }

    if (report->summary_file != 0) {
        fclose(report->summary_file);
        report->summary_file = 0;
    }

    if (report->runs_tsv_file != 0) {
        fclose(report->runs_tsv_file);
        report->runs_tsv_file = 0;
    }
}

int LFReportWriteRun(
    LFReport *report,
    const char *target_path,
    const char *input_path,
    const LFRunResult *result
)
{
    if (report == 0 || report->runs_tsv_file == 0) {
        return -1;
    }

    LFPrintTsvResult(report->runs_tsv_file, target_path, input_path, result);

    if (fflush(report->runs_tsv_file) != 0) {
        return -1;
    }

    return 0;
}

int LFReportWriteSummary(
    LFReport *report,
    const char *target_path,
    unsigned long total_runs,
    unsigned long ok_runs,
    unsigned long rejected_runs,
    unsigned long findings
)
{
    if (report == 0 || report->summary_file == 0) {
        return -1;
    }

    fprintf(report->summary_file, "LEOFUZZ:SUMMARY target=%s runs=%lu ok=%lu rejected=%lu findings=%lu\n",
        target_path,
        total_runs,
        ok_runs,
        rejected_runs,
        findings
    );

    if (fflush(report->summary_file) != 0) {
        return -1;
    }

    return 0;
}
