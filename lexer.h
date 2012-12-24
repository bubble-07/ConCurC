#ifndef LEXER_DEFINED
#define LEXER_DEFINED

#include <stdlib.h>
#include "libs/dynstring.h"
typedef union {
    double floatval;
    string stringval;
    int intval;
} lexattribute;

typedef struct {
    int tokenval;
    lexattribute attr;
} lexid;

#define LPAREN 1
#define RPAREN 2
#define STRING 3
#define FLOAT 4
#define INT 5
#define DEF 6
#define LAMBDA 7
#define NAMESPACE 8
#define IMPORT 9
#define TYPE 10
#define SUBS 11
#define SUPS 12
#define EXPR 13

#define D_LEX(name) const static lexid name##_LEXID = {name, 0};

D_LEX(LPAREN)
D_LEX(RPAREN)
D_LEX(STRING)
D_LEX(FLOAT)
D_LEX(INT)
D_LEX(DEF)
D_LEX(LAMBDA)
D_LEX(NAMESPACE)
D_LEX(IMPORT)
D_LEX(TYPE)
D_LEX(SUBS)
D_LEX(SUPS)
D_LEX(EXPR)

const static lexid lexid_lookup_failure = {0, 0};

//NOTE: spooky action-at a distance that lets us compare lexid_eq values for in dicts
inline static int lexid_eq(lexid one, lexid two) {
    if (one.tokenval == two.tokenval) {
        return 1;
    }
    return 0;
}

DEFINE_DICT(string, lexid)


//LEXER: Input -- a string
//Output -- a dynamic array of lexids [program text] AND a dynarray of strings [symbols to names]
DEFINE_DYNARRAY(lexid)
DEFINE_DYNARRAY(string)
typedef struct {
    lexid_dynarray program;
    string_dynarray backsymtable;
} lex_result; 

lex_result lex();

#endif 
