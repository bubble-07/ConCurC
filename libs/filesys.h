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

static path path_lookup_failure = NULL;

static inline path string_to_path(string in) {
    path result = to_cstring(in);
    return result;
}

static inline string path_to_string(path in) {
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

static inline path cat_paths(path one, path two) {
    path result = strcat(strcpy(malloc(sizeof(char) * strlen(one)), one), "/");
    return strcat(result, two);
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

static inline path get_parent_dir(const_path file) {
    size_t i;
    for (i=strlen(file) - 1; i && (file[i] != '/'); i--);
    return strncpy(malloc(sizeof(char) * (i + 1)), file, i);
}
static inline path go_up_dir(const_path dir) {
    path result = strcat(strcpy(malloc(sizeof(char) * strlen(dir)), dir),"/..");
    MUTATE(path, result, realpath(result, NULL));
    return result;
}

static inline void get_parent_dirs_to_main(const_path file) {
    path currentpath = realpath(file, NULL);
    if (currentpath == NULL) {
        //PANIC!
        return;
    }
    MUTATE(path, currentpath, get_parent_dir(currentpath));
    DIR* dirp = opendir(currentpath);
    path prevpath = malloc(sizeof(char) * strlen(currentpath)); 

    while (!(dir_contains(dirp, "main.cur") | dir_contains(dirp, "main.ccur")) &&
            strcmp(prevpath, currentpath)) {

        strcpy(prevpath, currentpath);
        MUTATE(path, currentpath, go_up_dir(currentpath))
        closedir(dirp);
        dirp = opendir(currentpath);
    }
    free(currentpath);
    free(prevpath);
    closedir(dirp);
    return;
}
#endif
