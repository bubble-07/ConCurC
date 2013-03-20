#include <stdlib.h>
#include <stdio.h>
#include "libs/dynstring.h"

typedef struct {
    size_t lineno;
    string file;
    size_t linepos;
    FILE* fileptr;
} fileLoc;

inline static fileLoc* load_file(const char* name) {
    fileLoc* result = memalloc(sizeof(fileLoc));
    result->fileptr = fopen(name, "r");
    if (result->fileptr == NULL) {
        printf("%s", "WTF?");
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
