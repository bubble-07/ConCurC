#ifndef FILESYS_DEFINED
#define FILESYS_DEFINED
typedef char* path;
typedef const char* const_path;

static inline int dir_contains(DIR* dirp, path file) {
    struct dirent* entry;
    while ((entry = readdir(dirp)) != NULL) {
        if (!strcmp(entry->d_name, file)) {
            rewinddir(dirp);
            return 1;
        }
    }
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
