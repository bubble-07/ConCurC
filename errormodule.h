#include "libs/dynstring.h"

typedef struct {
    size_t lineno;
    string file;
    size_t linepos;
    FILE* fileptr;
} fileLoc;

#define LINE_END 1000
