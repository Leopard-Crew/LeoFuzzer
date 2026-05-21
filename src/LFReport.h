#ifndef LF_REPORT_H
#define LF_REPORT_H

#include "LFResult.h"

#include <stdio.h>

typedef struct LFReport {
    FILE *summary_file;
    FILE *runs_tsv_file;
    char *results_path;
    char *findings_path;
    unsigned long finding_artifact_count;
} LFReport;

int LFReportOpen(LFReport *report, const char *results_path);
void LFReportClose(LFReport *report);

int LFReportWriteRun(
    LFReport *report,
    const char *target_path,
    const char *input_path,
    const LFRunResult *result
);

int LFReportWriteFindingArtifact(
    LFReport *report,
    const char *target_path,
    const char *input_path,
    const LFRunResult *result
);

int LFReportWriteSummary(
    LFReport *report,
    const char *target_path,
    unsigned long total_runs,
    unsigned long ok_runs,
    unsigned long rejected_runs,
    unsigned long findings
);

#endif
