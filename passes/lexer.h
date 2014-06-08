#ifndef LEXER_DEFINED
#define LEXER_DEFINED

#include <stdlib.h>
#include "../prims/lexid.h"
#include "../prims/nametable.h"

//LEXER: Input -- a string
//Output -- a dynamic array of lexids [program text] AND a name table (for backwards lookup)
typedef struct {
    lexid_dynarray program;
    nametable names;
    string file;
} lex_result; 

/*lex() reads characters from stdin, and returns the above structure. Terminates upon EOF,
EOS, and currently upon \n FIXME: allow reading files by filename */
lex_result lex(fileLoc* file);

#endif 
