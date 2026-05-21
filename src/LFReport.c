#include "LFReport.h"

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

static char *LFReportStringDuplicate(const char *text)
{
    size_t length;
    char *copy;

    if (text == 0) {
        return 0;
    }

    length = strlen(text);
    copy = (char *)malloc(length + 1);

    if (copy == 0) {
        return 0;
    }

    memcpy(copy, text, length + 1);
    return copy;
}

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

static int LFReportCopyFile(const char *source_path, const char *destination_path)
{
    FILE *source;
    FILE *destination;
    unsigned char buffer[8192];
    size_t bytes_read;

    if (source_path == 0 || destination_path == 0) {
        return -1;
    }

    source = fopen(source_path, "rb");

    if (source == 0) {
        return -1;
    }

    destination = fopen(destination_path, "wb");

    if (destination == 0) {
        fclose(source);
        return -1;
    }

    for (;;) {
        bytes_read = fread(buffer, 1, sizeof(buffer), source);

        if (bytes_read > 0) {
            if (fwrite(buffer, 1, bytes_read, destination) != bytes_read) {
                fclose(destination);
                fclose(source);
                return -1;
            }
        }

        if (bytes_read < sizeof(buffer)) {
            if (ferror(source)) {
                fclose(destination);
                fclose(source);
                return -1;
            }

            break;
        }
    }

    if (fclose(destination) != 0) {
        fclose(source);
        return -1;
    }

    fclose(source);
    return 0;
}

static char *LFReportMakeFindingFileName(
    unsigned long index,
    const char *kind_name,
    const char *suffix
)
{
    char name[256];
    int written;

    if (kind_name == 0 || suffix == 0) {
        return 0;
    }

    written = snprintf(name, sizeof(name), "%06lu-%s.%s", index, kind_name, suffix);

    if (written < 0 || (size_t)written >= sizeof(name)) {
        return 0;
    }

    return LFReportStringDuplicate(name);
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
    report->results_path = 0;
    report->findings_path = 0;
    report->finding_artifact_count = 0;

    if (LFReportEnsureDirectory(results_path) != 0) {
        return -1;
    }

    report->results_path = LFReportStringDuplicate(results_path);

    if (report->results_path == 0) {
        LFReportClose(report);
        return -1;
    }

    report->findings_path = LFReportJoinPath(results_path, "findings");

    if (report->findings_path == 0) {
        LFReportClose(report);
        return -1;
    }

    summary_path = LFReportJoinPath(results_path, "summary.txt");

    if (summary_path == 0) {
        LFReportClose(report);
        return -1;
    }

    runs_path = LFReportJoinPath(results_path, "runs.tsv");

    if (runs_path == 0) {
        free(summary_path);
        LFReportClose(report);
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

    if (report->results_path != 0) {
        free(report->results_path);
        report->results_path = 0;
    }

    if (report->findings_path != 0) {
        free(report->findings_path);
        report->findings_path = 0;
    }

    report->finding_artifact_count = 0;
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

int LFReportWriteFindingArtifact(
    LFReport *report,
    const char *target_path,
    const char *input_path,
    const LFRunResult *result
)
{
    unsigned long artifact_index;
    const char *kind_name;
    char *metadata_name;
    char *input_name;
    char *metadata_path;
    char *input_copy_path;
    FILE *metadata;
    int input_copy_result;

    if (report == 0 || input_path == 0 || result == 0) {
        return -1;
    }

    if (report->findings_path == 0) {
        return -1;
    }

    if (LFReportEnsureDirectory(report->findings_path) != 0) {
        return -1;
    }

    report->finding_artifact_count++;
    artifact_index = report->finding_artifact_count;
    kind_name = LFResultKindName(result->kind);

    metadata_name = LFReportMakeFindingFileName(artifact_index, kind_name, "txt");
    input_name = LFReportMakeFindingFileName(artifact_index, kind_name, "input");

    if (metadata_name == 0 || input_name == 0) {
        free(metadata_name);
        free(input_name);
        return -1;
    }

    metadata_path = LFReportJoinPath(report->findings_path, metadata_name);
    input_copy_path = LFReportJoinPath(report->findings_path, input_name);

    free(metadata_name);
    free(input_name);

    if (metadata_path == 0 || input_copy_path == 0) {
        free(metadata_path);
        free(input_copy_path);
        return -1;
    }

    input_copy_result = LFReportCopyFile(input_path, input_copy_path);

    metadata = fopen(metadata_path, "w");

    if (metadata == 0) {
        free(metadata_path);
        free(input_copy_path);
        return -1;
    }

    fprintf(metadata, "LEOFUZZ:FINDING\n");
    fprintf(metadata, "kind=%s\n", kind_name);
    fprintf(metadata, "target=%s\n", target_path != 0 ? target_path : "");
    fprintf(metadata, "input=%s\n", input_path);
    fprintf(metadata, "input_copy=%s\n", input_copy_path);
    fprintf(metadata, "input_copy_status=%s\n", input_copy_result == 0 ? "OK" : "FAILED");
    fprintf(metadata, "exit_code=%d\n", result->exit_code);
    fprintf(metadata, "signal=%d\n", result->signal_number);
    fprintf(metadata, "timeout=%d\n", result->timed_out);
    fprintf(metadata, "elapsed_ms=%.3f\n", result->elapsed_ms);

    if (fclose(metadata) != 0) {
        free(metadata_path);
        free(input_copy_path);
        return -1;
    }

    free(metadata_path);
    free(input_copy_path);

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
