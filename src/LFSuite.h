#ifndef LF_SUITE_H
#define LF_SUITE_H

typedef struct LFSuiteSpec {
    char *name;
    char *target_path;
    char *input_path;
    char *corpus_path;
    char *results_path;

    int has_timeout_seconds;
    int timeout_seconds;

    int has_expect_runs;
    int has_expect_ok;
    int has_expect_rejected;
    int has_expect_findings;

    unsigned long expect_runs;
    unsigned long expect_ok;
    unsigned long expect_rejected;
    unsigned long expect_findings;
} LFSuiteSpec;

void LFSuiteSpecInit(LFSuiteSpec *suite);
void LFSuiteSpecClear(LFSuiteSpec *suite);

int LFSuiteLoad(
    LFSuiteSpec *suite,
    const char *suite_path
);

#endif
