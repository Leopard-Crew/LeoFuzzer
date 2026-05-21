#include "LFSuite.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char *LFSuiteStringDuplicate(const char *text)
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

static char *LFSuiteTrim(char *text)
{
    char *end;

    if (text == 0) {
        return 0;
    }

    while (*text != '\0' && isspace((unsigned char)*text)) {
        text++;
    }

    if (*text == '\0') {
        return text;
    }

    end = text + strlen(text) - 1;

    while (end > text && isspace((unsigned char)*end)) {
        *end = '\0';
        end--;
    }

    return text;
}

static void LFSuiteStripLineEnding(char *line)
{
    size_t length;

    if (line == 0) {
        return;
    }

    length = strlen(line);

    while (length > 0) {
        if (line[length - 1] != '\n' && line[length - 1] != '\r') {
            break;
        }

        line[length - 1] = '\0';
        length--;
    }
}

static int LFSuiteSetString(char **destination, const char *value)
{
    char *copy;

    if (destination == 0 || value == 0) {
        return -1;
    }

    copy = LFSuiteStringDuplicate(value);

    if (copy == 0) {
        return -1;
    }

    free(*destination);
    *destination = copy;

    return 0;
}

static int LFSuiteParseUnsignedLong(const char *text, unsigned long *value)
{
    char *end;
    unsigned long parsed;

    if (text == 0 || value == 0) {
        return -1;
    }

    parsed = strtoul(text, &end, 10);

    if (end == text || *end != '\0') {
        return -1;
    }

    *value = parsed;
    return 0;
}

static int LFSuiteParseInt(const char *text, int *value)
{
    unsigned long parsed;

    if (LFSuiteParseUnsignedLong(text, &parsed) != 0) {
        return -1;
    }

    *value = (int)parsed;
    return 0;
}

static int LFSuiteApplyKeyValue(
    LFSuiteSpec *suite,
    const char *key,
    const char *value
)
{
    if (strcmp(key, "name") == 0) {
        return LFSuiteSetString(&suite->name, value);
    }

    if (strcmp(key, "target") == 0) {
        return LFSuiteSetString(&suite->target_path, value);
    }

    if (strcmp(key, "input") == 0) {
        return LFSuiteSetString(&suite->input_path, value);
    }

    if (strcmp(key, "corpus") == 0) {
        return LFSuiteSetString(&suite->corpus_path, value);
    }

    if (strcmp(key, "results") == 0) {
        return LFSuiteSetString(&suite->results_path, value);
    }

    if (strcmp(key, "timeout") == 0) {
        if (LFSuiteParseInt(value, &suite->timeout_seconds) != 0) {
            return -1;
        }

        suite->has_timeout_seconds = 1;
        return 0;
    }

    if (strcmp(key, "expect_runs") == 0) {
        if (LFSuiteParseUnsignedLong(value, &suite->expect_runs) != 0) {
            return -1;
        }

        suite->has_expect_runs = 1;
        return 0;
    }

    if (strcmp(key, "expect_ok") == 0) {
        if (LFSuiteParseUnsignedLong(value, &suite->expect_ok) != 0) {
            return -1;
        }

        suite->has_expect_ok = 1;
        return 0;
    }

    if (strcmp(key, "expect_rejected") == 0) {
        if (LFSuiteParseUnsignedLong(value, &suite->expect_rejected) != 0) {
            return -1;
        }

        suite->has_expect_rejected = 1;
        return 0;
    }

    if (strcmp(key, "expect_findings") == 0) {
        if (LFSuiteParseUnsignedLong(value, &suite->expect_findings) != 0) {
            return -1;
        }

        suite->has_expect_findings = 1;
        return 0;
    }

    return -1;
}

void LFSuiteSpecInit(LFSuiteSpec *suite)
{
    if (suite == 0) {
        return;
    }

    memset(suite, 0, sizeof(*suite));
}

void LFSuiteSpecClear(LFSuiteSpec *suite)
{
    if (suite == 0) {
        return;
    }

    free(suite->name);
    free(suite->target_path);
    free(suite->input_path);
    free(suite->corpus_path);
    free(suite->results_path);

    memset(suite, 0, sizeof(*suite));
}

int LFSuiteLoad(
    LFSuiteSpec *suite,
    const char *suite_path
)
{
    FILE *file;
    char line[4096];
    char *trimmed;
    char *equals;
    char *key;
    char *value;
    unsigned long line_number;

    if (suite == 0 || suite_path == 0) {
        return -1;
    }

    LFSuiteSpecClear(suite);

    file = fopen(suite_path, "r");

    if (file == 0) {
        return -1;
    }

    line_number = 0;

    while (fgets(line, sizeof(line), file) != 0) {
        line_number++;
        LFSuiteStripLineEnding(line);
        trimmed = LFSuiteTrim(line);

        if (*trimmed == '\0') {
            continue;
        }

        if (*trimmed == '#') {
            continue;
        }

        equals = strchr(trimmed, '=');

        if (equals == 0) {
            fprintf(stderr, "LEOFUZZ:SUITE_ERROR line=%lu reason=missing-equals\n", line_number);
            fclose(file);
            return -1;
        }

        *equals = '\0';
        key = LFSuiteTrim(trimmed);
        value = LFSuiteTrim(equals + 1);

        if (*key == '\0') {
            fprintf(stderr, "LEOFUZZ:SUITE_ERROR line=%lu reason=empty-key\n", line_number);
            fclose(file);
            return -1;
        }

        if (LFSuiteApplyKeyValue(suite, key, value) != 0) {
            fprintf(stderr, "LEOFUZZ:SUITE_ERROR line=%lu key=%s\n", line_number, key);
            fclose(file);
            return -1;
        }
    }

    if (ferror(file)) {
        fclose(file);
        return -1;
    }

    fclose(file);
    return 0;
}
