#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dict.h"
//setup for creating new dictionary type
DEFINE_DYNARRAY(char)
typedef char_dynarray string;

string to_dynstring(char* in) {
    int i = 0;
    string result = char_dynarray_make(1);
    while (in[i]) {
        result = char_dynarray_add(result, in[i]);
        i++;
    }
    return result;
}

size_t hash_string(string in) {
    size_t result = 0;
    size_t i = 0;
    while (i < in.size) {
        result += in.begin[i];
        i++;
    }
    return result;
}

int string_eq(string one, string two) {
    size_t i = 0;
    if (one.size != two.size) {
        return 0;
    }
    while (i < one.size) {
        if (one.begin[i] != two.begin[i]) {
            return 0;
        }
        i++;
    }
    return 1;
}

typedef union {
    double floatval;
    string stringval;
    int intval;
} lexattribute;

typedef struct {
    int tokenval;
    lexattribute attr;
} lexid;

lexid LPAREN = {1, 0};
lexid RPAREN = {2, 0};
lexid STRINGID = {3, 0};
lexid FLOATID = {4, 0};
lexid INTID = {5, 0};
lexid lexid_lookup_failure = {0, 0};

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

    symtable = string_lexid_dict_add(symtable, string_lexid_bucket_make(to_dynstring("("), LPAREN));
    symtable = string_lexid_dict_add(symtable, string_lexid_bucket_make(to_dynstring(")"), RPAREN));
    int i = 0;
    int newlex = INTID.tokenval + 1;
    char current = getchar();
    while (current && current != EOF && current != '\0' && current != '\n') {
        string_lexid_bucket tmpbucket; 
        lexid tmpid;
        string id;
        switch (current) {
            case '(':
                program = lexid_dynarray_add(program, LPAREN);
                break;
            case ')':
                program = lexid_dynarray_add(program, RPAREN);
                break;
            case ' ':
                break;
            case '0': case '1': case '2': case '3': case '4':
            case '5': case '6': case '7': case '8': case '9':
                tmpid = INTID;
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
                    tmpid = FLOATID;
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
                
int main(int argc, const char * argv[]) {
    lex_result result = lex();
    int i = 0;
//    printf("%f", 40.0);
    while (i < result.program.size) {
        if (result.program.begin[i].tokenval == 4) {
            printf("%f", result.program.begin[i].attr.floatval);
        }
        if (result.program.begin[i].tokenval == 5) {
            printf("%d", result.program.begin[i].attr.intval);
        }
        else {
            printf("%d", (int)result.program.begin[i].tokenval);
        }
        printf("%s", "\n");
        i++;
    }
    return 0;
}
