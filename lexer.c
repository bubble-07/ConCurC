#include "lexer.h"
//LEXER -- lex() gets you a stream of tokens. First, some helper functions.

//takes the current character and whether it's supposed to be negative or not
//returns a lexid that represents either a float or an int
lexid lexNum(char current, int negative) {
    lexid tmpid = INT_LEXID;
    ungetc(current, stdin);
    scanf("%d", &tmpid.attr.intval);
    current = getchar();
    if (current != '.') {
        ungetc(current, stdin);
        if (negative) {
            tmpid.attr.intval = -tmpid.attr.intval;
        }
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
        if (negative) {
            resulting = -resulting;
        }
        tmpid.attr.floatval = resulting;
    }
    return tmpid;
}

int isNotGlobTerm(char c) {
    return c && c != EOF && c != '\0' && c != '\n';
}

int isNotTerm(char c) {
    return isNotGlobTerm(c) && c != '(' && c != ' ' && c != ')';
}

//takes the current character, a symbol table, and a reference to the new unique identifier num
//returns a lexid with the new identifier (if not seen before) or a lexid of an old one.
lexid lexIdentifier(char current, string_lexid_dict symtable, int* newlex) {
    lexid tmpid;
    char_dynarray id = to_dynstring("");
    while (isNotTerm(current)) {    
        id = char_dynarray_add(id, current);
        current = getchar();
    }
    ungetc(current, stdin);
    lexid lookupval = string_lexid_dict_get(symtable, id);
    if (lexid_eq(lookupval, lexid_lookup_failure)) {
        tmpid.tokenval = *newlex;
        tmpid.attr.intval = 0;
        symtable = string_lexid_dict_add(symtable, string_lexid_bucket_make(id, tmpid));
        lookupval = tmpid;
        *newlex = *newlex + 1;
    }
    return lookupval;
}


lexid lexString(char current) {
    lexid tmpid = STRING_LEXID;
    char_dynarray stringval = to_dynstring("");
    current = getchar();
    while (current != '"') {
        stringval = char_dynarray_add(stringval, current);
        current = getchar();
    }
    tmpid.attr.stringval = stringval;
    return tmpid;
}


#define ADDCONST_SYM(nstring, constid) symtable = \
string_lexid_dict_add(symtable, string_lexid_bucket_make(to_dynstring(nstring), constid))


lex_result lex() {
    string_lexid_dict symtable = string_lexid_dict_init(100);
    lexid_dynarray program = lexid_dynarray_make(100);

    ADDCONST_SYM("(", LPAREN_LEXID);
    ADDCONST_SYM(")", RPAREN_LEXID);
    int i = 0;
    int newlex = EXPR + 1;
    char current = getchar();
    while (isNotGlobTerm(current)) {
        lexid tmpid;
        while (current == ' ') {
            current = getchar();
        }
        switch (current) {
            case '(':
                tmpid =  LPAREN_LEXID;
                break;
            case ')':
                tmpid = RPAREN_LEXID;
                break;
            case '0': case '1': case '2': case '3': case '4':
            case '5': case '6': case '7': case '8': case '9':
                tmpid = lexNum(current, 0);
                break;
            case '"':
                tmpid = lexString(current);
                break;
            case '-':
                current = getchar();
                if (isdigit(current)) {
                    tmpid = lexNum(current, 1);
                    break;
                }
                ungetc(current, stdin);

            default:
                tmpid = lexIdentifier(current, symtable, &newlex);
                break;  
        }
        program = lexid_dynarray_add(program, tmpid);
        current = getchar();
    }
    lex_result result;
    result.program = program;
    return result;
}
