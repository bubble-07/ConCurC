#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include "../libs/dynstring.h"

void listallindir(DIR* dirp) {
    struct dirent* entry;
    while ((entry = readdir(dirp)) != NULL) {
        printf("%s", entry->d_name);
        printf("%s", "\n");
    }
    return;
}

int main() {
    string dirstring = to_dynstring(".");
    char* currentcstring = to_cstring(dirstring);
    char* prev = malloc(sizeof(char) + 1);
    prev[0] = '\0';
    char* tmp;
    DIR* dirp = opendir(currentcstring);
    while (dirp != NULL && (strcmp(prev, currentcstring))) {
        listallindir(dirp);
        closedir(dirp);
        free(prev);
        prev = malloc(sizeof(char) * (dirstring.size + 1));
        strcpy(prev, currentcstring);
        free(currentcstring);
        dirstring = char_dynarray_cat(dirstring, to_dynstring("/.."));
        currentcstring = to_cstring(dirstring); 
        tmp = realpath(currentcstring, NULL);
        currentcstring = tmp;
        dirp = opendir(currentcstring);
    }
    return;
}
