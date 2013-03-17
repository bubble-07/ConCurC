#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include "../libs/dynstring.h"
#include "../libs/dict.h"

#define MUTATE(type, var, val) type type##_tmp = val; free(var); var = type##_tmp;

typedef char* path;
typedef const char* const_path;

int dir_contains(DIR* dirp, path file) {
    struct dirent* entry;
    while ((entry = readdir(dirp)) != NULL) {
        if (!strcmp(entry->d_name, file)) {
            return 1;
        }
    }
    rewinddir(dirp);
    return 0;
}

path get_parent_dir(const_path file) {
    size_t i;
    for (i=strlen(file) - 1; i && (file[i] != '/'); i--);
    return strncpy(malloc(sizeof(char) * (i + 1)), file, i);
}
path go_up_dir(const_path dir) {
    path result = strcat(strcpy(malloc(sizeof(char) * strlen(dir)), dir),"/..");
    MUTATE(path, result, realpath(result, NULL));
    return result;
}

void get_parent_dirs_to_main(const_path file) {
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
int main(int argc, const char* argv[]) {
    get_parent_dirs_to_main(argv[1]);
    return 0;
}



