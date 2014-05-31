#include <stdlib.h>
#include <stdio.h>
#include "dynstring.h"
#include "error.h"
#include "filesys.h"

#ifndef FILEHANDLER_DEFINED
#define FILEHANDLER_DEFINED

typedef struct {
    size_t lineno;
    string file;
    size_t linepos;
    FILE* fileptr;
} fileLoc;

inline static fileLoc* load_file(const char* name) {
    printf("%s", name);
    fileLoc* result = memalloc(sizeof(fileLoc));
    DIR* test = opendir(name);
    if (test != NULL) {
        printf("%s \n", name);
        fatal_error("was attempted to be read as a file!");
    }
    result->fileptr = fopen(name, "r");
    if (result->fileptr == NULL) {
        printf("%s", "WTFONE?");
    }
    result->lineno = 1;
    result->linepos = 0;
    result->file = to_dynstring(name);
    return result;
}
inline static fileLoc* load_stdin() {
    fileLoc* result = memalloc(sizeof(fileLoc));
    result->fileptr = stdin;
    result->lineno = 1;
    result->linepos = 0;
    result->file = to_dynstring("stdin");
    return result;
}
inline static void close_file(fileLoc* in) {
    if (in->fileptr == stdin) {
        return;
    }
    fclose(in->fileptr);
    return;
}

#define LINE_END 1000

#endif
