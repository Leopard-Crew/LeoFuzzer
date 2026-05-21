#ifndef LF_REPLAY_H
#define LF_REPLAY_H

typedef struct LFReplaySpec {
    char *metadata_path;
    char *expected_kind;
    char *target_path;
    char *input_copy_path;
} LFReplaySpec;

void LFReplaySpecInit(LFReplaySpec *spec);
void LFReplaySpecClear(LFReplaySpec *spec);

int LFReplayLoad(
    LFReplaySpec *spec,
    const char *metadata_path
);

#endif
