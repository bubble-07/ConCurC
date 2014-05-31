#ifndef LEXER_DEFINED
#define LEXER_DEFINED

#include <stdlib.h>
#include "../prims/lexid.h"

/*defines a dictionary from strings to lexids in order to implement a working symbol table*/
DEFINE_DICT(string, lexid)


//LEXER: Input -- a string
//Output -- a dynamic array of lexids [program text] AND a dynarray of strings [symbols to names]
DEFINE_DYNARRAY(string)
typedef struct {
    lexid_dynarray program;
    /*a "backwards" symbol table that is actually just a dynarray of strings, with the
    identifier's number cleverly represented in the index*/
    string_dynarray backsymtable;
    string file;
} lex_result; 

/*lex() reads characters from stdin, and returns the above structure. Terminates upon EOF,
EOS, and currently upon \n FIXME: allow reading files by filename */
lex_result lex(fileLoc* file);

#endif 
