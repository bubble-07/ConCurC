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
#define EXPR 6

const lexid LPAREN_LEXID = {LPAREN, 0};
const lexid RPAREN_LEXID = {RPAREN, 0};
const lexid STRING_LEXID = {STRING, 0};
const lexid FLOAT_LEXID = {FLOAT, 0};
const lexid INT_LEXID = {INT, 0};
const lexid EXPR_LEXID = {EXPR, 0};
const lexid lexid_lookup_failure = {0, 0};

int lexid_eq(lexid one, lexid two) {
    if (one.tokenval == two.tokenval) {
        return 1;
    }
    return 0;
}

DEFINE_DICT(string, lexid)

//end setup

//LEXER: Input -- a string
//Output -- a dynamic array of lexids [program text] AND a dynarray of strings [symbols to names]
DEFINE_DYNARRAY(lexid)
DEFINE_DYNARRAY(string)
typedef struct {
    lexid_dynarray program;
    string_dynarray backsymtable;
} lex_result; 


