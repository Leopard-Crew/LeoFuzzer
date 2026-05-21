#include "LFCorpus.h"

#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

typedef struct LFCorpusList {
    char **paths;
    unsigned long count;
    unsigned long capacity;
} LFCorpusList;

static char *LFStringDuplicate(const char *text)
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

static char *LFJoinPath(const char *left, const char *right)
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

static int LFComparePaths(const void *left, const void *right)
{
    const char *const *left_path;
    const char *const *right_path;

    left_path = (const char *const *)left;
    right_path = (const char *const *)right;

    return strcmp(*left_path, *right_path);
}

static void LFFreeCorpusList(LFCorpusList *list)
{
    unsigned long index;

    if (list == 0) {
        return;
    }

    for (index = 0; index < list->count; index++) {
        free(list->paths[index]);
    }

    free(list->paths);
    list->paths = 0;
    list->count = 0;
    list->capacity = 0;
}

static int LFAppendCorpusPath(LFCorpusList *list, const char *path)
{
    char **new_paths;
    char *copy;
    unsigned long new_capacity;

    if (list == 0 || path == 0) {
        return -1;
    }

    if (list->count == list->capacity) {
        new_capacity = list->capacity == 0 ? 16 : list->capacity * 2;
        new_paths = (char **)realloc(list->paths, sizeof(char *) * new_capacity);

        if (new_paths == 0) {
            return -1;
        }

        list->paths = new_paths;
        list->capacity = new_capacity;
    }

    copy = LFStringDuplicate(path);

    if (copy == 0) {
        return -1;
    }

    list->paths[list->count] = copy;
    list->count++;

    return 0;
}

static int LFReadCorpusDirectory(const char *corpus_path, LFCorpusList *list)
{
    DIR *dir;
    struct dirent *entry;
    struct stat st;
    char *full_path;

    dir = opendir(corpus_path);

    if (dir == 0) {
        return -1;
    }

    for (;;) {
        entry = readdir(dir);

        if (entry == 0) {
            break;
        }

        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        full_path = LFJoinPath(corpus_path, entry->d_name);

        if (full_path == 0) {
            closedir(dir);
            return -1;
        }

        if (stat(full_path, &st) == 0) {
            if (S_ISREG(st.st_mode)) {
                if (LFAppendCorpusPath(list, full_path) != 0) {
                    free(full_path);
                    closedir(dir);
                    return -1;
                }
            }
        }

        free(full_path);
    }

    closedir(dir);
    return 0;
}

int LFVisitCorpusFiles(
    const char *corpus_path,
    LFCorpusVisitor visitor,
    void *user_data
)
{
    LFCorpusList list;
    unsigned long index;
    int visit_result;

    if (corpus_path == 0 || visitor == 0) {
        return -1;
    }

    list.paths = 0;
    list.count = 0;
    list.capacity = 0;

    if (LFReadCorpusDirectory(corpus_path, &list) != 0) {
        LFFreeCorpusList(&list);
        return -1;
    }

    qsort(list.paths, list.count, sizeof(char *), LFComparePaths);

    for (index = 0; index < list.count; index++) {
        visit_result = visitor(list.paths[index], user_data);

        if (visit_result != 0) {
            LFFreeCorpusList(&list);
            return visit_result;
        }
    }

    LFFreeCorpusList(&list);
    return 0;
}
