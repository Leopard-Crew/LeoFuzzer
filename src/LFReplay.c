#include "LFReplay.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char *LFReplayStringDuplicate(const char *text)
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

static void LFReplayStripLineEnding(char *line)
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

static int LFReplaySetString(char **destination, const char *value)
{
    char *copy;

    if (destination == 0 || value == 0) {
        return -1;
    }

    copy = LFReplayStringDuplicate(value);

    if (copy == 0) {
        return -1;
    }

    free(*destination);
    *destination = copy;

    return 0;
}

static int LFReplayReadKeyValue(LFReplaySpec *spec, const char *line)
{
    const char *value;

    if (spec == 0 || line == 0) {
        return -1;
    }

    if (strncmp(line, "kind=", 5) == 0) {
        value = line + 5;
        return LFReplaySetString(&spec->expected_kind, value);
    }

    if (strncmp(line, "target=", 7) == 0) {
        value = line + 7;
        return LFReplaySetString(&spec->target_path, value);
    }

    if (strncmp(line, "input_copy=", 11) == 0) {
        value = line + 11;
        return LFReplaySetString(&spec->input_copy_path, value);
    }

    return 0;
}

void LFReplaySpecInit(LFReplaySpec *spec)
{
    if (spec == 0) {
        return;
    }

    spec->metadata_path = 0;
    spec->expected_kind = 0;
    spec->target_path = 0;
    spec->input_copy_path = 0;
}

void LFReplaySpecClear(LFReplaySpec *spec)
{
    if (spec == 0) {
        return;
    }

    free(spec->metadata_path);
    free(spec->expected_kind);
    free(spec->target_path);
    free(spec->input_copy_path);

    spec->metadata_path = 0;
    spec->expected_kind = 0;
    spec->target_path = 0;
    spec->input_copy_path = 0;
}

int LFReplayLoad(
    LFReplaySpec *spec,
    const char *metadata_path
)
{
    FILE *file;
    char line[4096];

    if (spec == 0 || metadata_path == 0) {
        return -1;
    }

    LFReplaySpecClear(spec);

    if (LFReplaySetString(&spec->metadata_path, metadata_path) != 0) {
        return -1;
    }

    file = fopen(metadata_path, "r");

    if (file == 0) {
        return -1;
    }

    while (fgets(line, sizeof(line), file) != 0) {
        LFReplayStripLineEnding(line);

        if (LFReplayReadKeyValue(spec, line) != 0) {
            fclose(file);
            return -1;
        }
    }

    if (ferror(file)) {
        fclose(file);
        return -1;
    }

    fclose(file);

    if (spec->target_path == 0 || spec->input_copy_path == 0) {
        return -1;
    }

    return 0;
}
