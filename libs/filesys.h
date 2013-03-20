#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include "dynstring.h"
#ifndef FILESYS_DEFINED
#define FILESYS_DEFINED

#define MUTATE(type, var, val) type type##_tmp = val; free(var); var = type##_tmp;
typedef char* path;
typedef const char* const_path;

DEFINE_DYNARRAY(path)

static path path_lookup_failure = NULL;

static inline path string_to_path(string in) {
    path result = to_cstring(in);
    return result;
}

static inline string path_to_string(const_path in) {
    string result = to_dynstring(in);
    return result;
}

static inline int path_eq(path one, path two) {
    return !strcmp(one, two);
}
static inline size_t hash_path(path in) {
    string tmp = path_to_string(in);
    size_t result = hash_string(tmp);
    char_dynarray_free(tmp);
    return result;
}
static inline path copy_path(path in) {
    return strcpy(malloc(strlen(in) + 1), in);
}

static inline path cat_paths(path one, path two) {
    path result = strcat(strcpy(malloc(sizeof(char) * (strlen(one) + 2)), one), "/");
    result = strcat(result, two);
    path resultpath = strcpy(malloc(strlen(result) + 1), result);
    free(result);
    return resultpath;
}

static inline path cat_extn(path one, path extn) {
    path result = strcat(strcpy(malloc(sizeof(char) * strlen(one)), one), ".");
    return strcat(result, extn);
}
static inline void path_free(path in) {
    free(in);
    return;
}

//NOTE: must make it also match ".cur" and ".ccur" extenstions
static inline int dir_contains(DIR* dirp, path file) {
    path dotccur = cat_extn(file, "cur");
    path dotcur = cat_extn(file, "ccur");
    struct dirent* entry;
    while ((entry = readdir(dirp)) != NULL) {
        if (!strcmp(entry->d_name, file) || !strcmp(entry->d_name, dotcur) ||
                !strcmp(entry->d_name, dotccur)) {
            rewinddir(dirp);
            path_free(dotcur);
            path_free(dotccur);
            return 1;
        }
    }
    path_free(dotcur);
    path_free(dotccur);
    rewinddir(dirp);
    return 0;
}
static inline path get_file_extn(DIR* dirp, path file) {
    path dotccur = cat_extn(file, "cur");
    path dotcur = cat_extn(file, "ccur");
    struct dirent* entry;
    while ((entry = readdir(dirp)) != NULL) {
        if (!strcmp(entry->d_name, dotcur)) {
            rewinddir(dirp);
            path_free(dotccur);
            return dotcur;
        }
        if (!strcmp(entry->d_name, dotccur)) {
            rewinddir(dirp);
            path_free(dotcur);
            return dotccur;
        }
    }
    path_free(dotcur);
    path_free(dotccur);
    rewinddir(dirp);
    return strcpy(malloc(strlen(file) + 1), file);
}


static inline path get_parent_dir(const_path file) {
    size_t i;
    for (i=strlen(file) - 1; i && (file[i] != '/'); i--);
    return strncpy(malloc(sizeof(char) * (i + 1)), file, i);
}
static inline path get_innermost_dir(const_path file) {
    size_t i;
    for (i=strlen(file) - 1; i && (file[i] != '/'); i--);
    i++;
    return strcpy(malloc(strlen(file) - i), file + i);
}

static inline int is_within(const_path file, path folder) {
    size_t i;
    if (strlen(file) < strlen(folder)) {
        return 0;
    }
    if (!strcmp(file,folder)) {
        return 0;
    }
    for (i=0; i < strlen(folder); i++) {
        if (file[i] != folder[i]) {
            return 0;
        }
    }
    return 1;
}

static inline int is_within_anyof(const_path file, path_dynarray folders) {
    size_t i;
    for (i=0; i < folders.size; i++) {
        if (is_within(file, folders.begin[i])) {
            return 1;
        }
    }
    return 0;
}

static inline path_dynarray get_parent_dirs_to_root(const_path file, path_dynarray folders) {
    path_dynarray result = path_dynarray_make(1);
    path currentpath = realpath(file, NULL);
    if (currentpath == NULL) {
        //PANIC!
        return;
    }
    MUTATE(path, currentpath, get_parent_dir(currentpath))
    while (is_within_anyof(currentpath, folders)) {
        result = path_dynarray_add(result, copy_path(currentpath));
        MUTATE(path, currentpath, get_parent_dir(currentpath));
    }
    free(currentpath);
    return result;
}
#endif
