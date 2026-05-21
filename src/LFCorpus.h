#ifndef LF_CORPUS_H
#define LF_CORPUS_H

typedef int (*LFCorpusVisitor)(
    const char *file_path,
    void *user_data
);

int LFVisitCorpusFiles(
    const char *corpus_path,
    LFCorpusVisitor visitor,
    void *user_data
);

#endif
