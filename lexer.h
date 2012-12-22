#include <stdlib.h>
#include "dynstring.h"
#ifndef LEXER_DEFINED
#define LEXER_DEFINED

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

lex_result lex() {
    string_lexid_dict symtable = string_lexid_dict_init(100);
    lexid_dynarray program = lexid_dynarray_make(100);

    symtable = string_lexid_dict_add(symtable, string_lexid_bucket_make(to_dynstring("("), LPAREN_LEXID));
    symtable = string_lexid_dict_add(symtable, string_lexid_bucket_make(to_dynstring(")"), RPAREN_LEXID));
    int i = 0;
    int newlex = EXPR + 1;
    char current = getchar();
    while (current && current != EOF && current != '\0' && current != '\n') {
        string_lexid_bucket tmpbucket; 
        lexid tmpid;
        string id;
        switch (current) {
            case '(':
                program = lexid_dynarray_add(program, LPAREN_LEXID);
                break;
            case ')':
                program = lexid_dynarray_add(program, RPAREN_LEXID);
                break;
            case ' ':
                break;
            case '0': case '1': case '2': case '3': case '4':
            case '5': case '6': case '7': case '8': case '9':
                tmpid = INT_LEXID;
                ungetc(current, stdin);
                scanf("%d", &tmpid.attr.intval);
                current = getchar();
                if (current != '.') {
                    ungetc(current, stdin);
                }
                else {
                    int afterdecimal;
                    double nonintegral;
                    scanf("%d", &afterdecimal);
                    nonintegral = (double)afterdecimal;
                    while (nonintegral > 1.0) {
                        nonintegral = nonintegral * 0.1;
                    }
                    double resulting = (double) tmpid.attr.intval + nonintegral;
                    tmpid = FLOAT_LEXID;
                    tmpid.attr.floatval = resulting;
                }

                program = lexid_dynarray_add(program, tmpid);
                break;
            default:
                id = to_dynstring("");
                while (current != ((char) 0) && current != '(' && current != '\0' && current != '\n'
                      && current != ')' && current != ' ' && current != EOF) {    
                    id = char_dynarray_add(id, current);
                    current = getchar();
                }
                ungetc(current, stdin);
                lexid lookupval = string_lexid_dict_get(symtable, id);
                if (lexid_eq(lookupval, lexid_lookup_failure)) {
                    tmpid.tokenval = newlex;
                    tmpid.attr.intval = 0;
                    tmpbucket = string_lexid_bucket_make(id, tmpid);
                    symtable = string_lexid_dict_add(symtable, tmpbucket);
                    lookupval = tmpid;
                    newlex++;
                }
                program = lexid_dynarray_add(program, lookupval);
                break;  
        }
        current = getchar();
    }
    lex_result result;
    result.program = program;
    return result;
}
#endif 