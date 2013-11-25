#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include "dynstring.h"
#ifndef FILESYS_DEFINED
#define FILESYS_DEFINED

//define a handy macro that replaces a c string with one that has been newly allocated
#define MUTATE(type, var, val) type type##_tmp = val; free(var); var = type##_tmp;

extern char* realpath(const char *path, char* resolved_path);

//Yep, keeping it real simple
typedef char* path;
typedef const char* const_path;

DEFINE_DYNARRAY(path)


/*Conversion routines between strings and paths -- both allocate new memory*/
static inline path string_to_path(string in) {
    path result = to_cstring(in);
    return result;
}

static inline string path_to_string(const_path in) {
    string result = to_dynstring(in);
    return result;
}

/*All of these are needed to be able to define dictionaries with paths */
static path path_lookup_failure = NULL;

static inline int path_eq(path one, path two) {
    if (one == NULL && two != NULL) {
        return 0;
    }
    if (two == NULL && one != NULL) {
        return 0;
    }
    if (two == one) {
        return 1;
    }
    return !strcmp(one, two);
}
static inline size_t hash_path(path in) {
    string tmp = path_to_string(in);
    size_t result = hash_string(tmp);
    char_dynarray_free(tmp);
    return result;
}

/* Now for some operations */
static inline path copy_path(const_path in) {
    return strcpy(malloc(strlen(in) + 1), in);
}

/* Concatenate a path and a relative folder path [within the first arg]. */
static inline path cat_paths(path one, path two) {
  path result = strcat(strcpy(malloc(sizeof(char) * (strlen(one) + strlen(two) + 2)), one), "/");
    result = strcat(result, two);
    path resultpath = strcpy(malloc(strlen(result) + 1), result);
    free(result);
    return resultpath;
}

/* Concatenate an extension onto a file path */
static inline path cat_extn(const_path one, path extn) {
  path result = strcat(strcpy(malloc(sizeof(char) * (strlen(one) + strlen(extn) + 2)), one), ".");
    return strcat(result, extn);
}


static inline void path_free(path in) {
    free(in);
    return;
}

/* If the path is a relative path [not anchored anywhere], returns 1, else 0 */
static inline int path_is_rel(path in) {
    size_t i;
    for (i=0; i < strlen(in); i++) {
        if (in[i] == '/') {
            return 0;
        }
    }
    return 1;
}

/* Gets the name of "file" within "folder" with its extension [if any] appended */
static inline path get_file_extn(path file, path folder) {
    DIR* dirp = opendir(folder);
    path dotccur = cat_extn(file, "cur");
    path dotcur = cat_extn(file, "ccur");
    struct dirent* entry;
    while ((entry = readdir(dirp)) != NULL) {
        if (!strcmp(entry->d_name, dotcur)) {
            path_free(dotccur);
            closedir(dirp);
            return dotcur;
        }
        if (!strcmp(entry->d_name, dotccur)) {
            path_free(dotcur);
            closedir(dirp);
            return dotccur;
        }
    }
    path_free(dotcur);
    path_free(dotccur);
    closedir(dirp);
    return strcpy(malloc(strlen(file) + 1), file);
}

/* gets the parent directory of an absolute path*/
static inline path get_parent_dir(path file) {
    size_t i;
    for (i=strlen(file) - 1; i && (file[i] != '/'); i--);
    path tmp =  strncpy(malloc(sizeof(char) * (i + 1)), file, i);
    tmp[i] = '\0';
    return tmp;
}

/* gets the innermost directory of a path to a directory */
static inline path get_innermost_dir(const_path file) {
    size_t i;
    if (file == NULL) {
        return NULL;
    }
    for (i=strlen(file) - 1; i && (file[i] != '/'); i--);
    i++;
    return strcpy(malloc(strlen(file) - i + 2), file + i);
}

/* gets if a given file [sans extension] is within a folder's path */
static inline int rel_is_within(const_path file, path folder) {
    folder = copy_path(folder); //need to copy for opendir() (working hypothesis)
    DIR* dirp = opendir(folder);
    path_free(folder);
    path dotcur = cat_extn(file, "cur");
    path dotccur = cat_extn(file, "ccur");
    struct dirent* entry;
    while ((entry = readdir(dirp)) != NULL) {
        if (!strcmp(entry->d_name, file) || !strcmp(entry->d_name, dotcur) ||
                !strcmp(entry->d_name, dotccur)) {
            path_free(dotcur);
            path_free(dotccur);
            closedir(dirp);
            return 1;
        }
    }
    path_free(dotcur);
    path_free(dotccur);
    closedir(dirp);
    return 0;
}

/*Gets the folder path that "file" is within */
static inline path rel_get_within(const_path file, path_dynarray folders) {
    size_t i;
    for (i=0; i < folders.size; i++) {
        if (rel_is_within(file, folders.begin[i])) {
            return folders.begin[i];
        }
    }
    return path_lookup_failure;
}


/* if an absolute path "file" is within absolute path "folder", return 1, else 0 */
static inline int abs_is_within(const_path file, path folder) {
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

/* returns true if the given absolute file path is within any of the folders */
static inline int abs_is_within_anyof(const_path file, path_dynarray folders) {
    size_t i;
    for (i=0; i < folders.size; i++) {
        if (abs_is_within(file, folders.begin[i])) {
            return 1;
        }
    }
    return 0;
}

/*returns a dynamic array of paths that are parents of file, up to, but not including any of
 * the absolute directory paths in "folders" */
static inline path_dynarray get_parent_dirs_to_root(const_path file, path_dynarray folders) {
    path_dynarray result = path_dynarray_make(1);
    path currentpath = realpath(file, NULL);
    if (currentpath == NULL) {
        //PANIC!
        return result;
    }
    MUTATE(path, currentpath, get_parent_dir(currentpath))
    while (abs_is_within_anyof(currentpath, folders)) {
        result = path_dynarray_add(result, copy_path(currentpath));
        MUTATE(path, currentpath, get_parent_dir(currentpath));
    }
    free(currentpath);
    return result;
}
#endif
