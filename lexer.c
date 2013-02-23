#include "lexer.h"
#include "math.h"
//LEXER -- lex() gets you a stream of tokens. First, some helper functions.

//takes the current character and whether it's supposed to be negative or not
//returns a lexid that represents either a float or an int
lexid lexNum(char current, int negative, FILE * file) {
    lexid tmpid = INT_LEXID;
    ungetc(current, file);
    fscanf(file, "%d", &tmpid.attr.intval);
    current = getc(file);
    if (current != '.') {
        ungetc(current, file);
        if (negative) {
            tmpid.attr.intval = -tmpid.attr.intval;
        }
    }
    else {
        int afterdecimal;
        double nonintegral;
        fscanf(file, "%d", &afterdecimal);
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

int isNewline(char c) {
    return c == '\n' || c == '\r';
}

int isNotTerm(char c) {
    return isNotGlobTerm(c) && c != '(' && c != ' ' && c != ')' && c != ',' && c != '\t' &&
        c != '.';
}

//takes the current character, a symbol table, and a reference to the new unique identifier num
//returns a lexid with the new identifier (if not seen before) or a lexid of an old one.
lexid lexIdentifier(char current, string_lexid_dict symtable, int* newlex, FILE* file) {
    lexid tmpid;
    char_dynarray id = to_dynstring("");
    printf("%s", " ");
    printf("%d", (int) current);
    printf("%s", "    ");
    while (isNotTerm(current)) {    
        id = char_dynarray_add(id, current);
        current = getc(file);
    }
    ungetc(current, file);
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

//Lexes a string literal
lexid lexString(char current, FILE * file) {
    lexid tmpid = STRING_LEXID;
    char_dynarray stringval = to_dynstring("");
    current = getc(file);
    while (current != '"') {
        stringval = char_dynarray_add(stringval, current);
        current = getc(file);
    }
    tmpid.attr.stringval = stringval;
    return tmpid;
}

//convenience macro to fill the symtable with core forms [primitives] so their token values
//are predictable
#define ADDCONST_SYM(nstring, constid) symtable = \
string_lexid_dict_add(symtable, string_lexid_bucket_make(to_dynstring(nstring), constid))


lex_result lex(FILE * file) {
    string_lexid_dict symtable = string_lexid_dict_init(100);
    lexid_dynarray program = lexid_dynarray_make(100);

    int repl = 0; //variable that is 1 if file is stdin, else 0
    if (file == stdin) {
        repl = 1;
    }

    /*filling the symtable with core forms*/
    ADDCONST_SYM("def", DEF_LEXID);
    ADDCONST_SYM("lambda", LAMBDA_LEXID);
    ADDCONST_SYM("namespace", NAMESPACE_LEXID);
    ADDCONST_SYM("import", IMPORT_LEXID);
    ADDCONST_SYM("type", TYPE_LEXID);

    ADDCONST_SYM("<:", SUBS_LEXID);
    ADDCONST_SYM("subs", SUBS_LEXID);
    ADDCONST_SYM("subtypes", SUBS_LEXID);

    ADDCONST_SYM("sups", SUPS_LEXID);
    ADDCONST_SYM(":>", SUPS_LEXID);
    ADDCONST_SYM("supertypes", SUPS_LEXID);

    /*Meat of the actual lexer. Conceptually, it does this by predictive decisions using the 
    current character as a reference.*/
    int i = 0;
    int currentindent = 0;
    int newlex = EXPR + 1;
    char current = getc(file);
    while (isNotGlobTerm(current) || (!repl && isNewline(current))) {
        lexid tmpid;
        //Consume until we get something that isn't a space
        while (current == ' ') {
            current = getc(file);
        }
        switch (current) {
            case '(':
                tmpid =  LPAREN_LEXID;
                break;
            case ')':
                tmpid = RPAREN_LEXID;
                break;
            case ',':
                tmpid = COMMA_LEXID;
                break;
            case '.':
                tmpid = DOT_LEXID;
                break;
            case '\n': case '\r':
                printf("%s", "BLAHHHHH");
                tmpid = NEWLINE_LEXID;
                current = getc(file);
                float tmpindent;
                for (tmpindent = 0; 
                     current == ' ' || current == '\t'; current = getc(file)) {
                    if (current == '\t') {
                        tmpindent = tmpindent + 1.0000;
                    }
                    if (current == ' ') {
                        tmpindent = tmpindent + 0.2500;
                    }
                }
                int newindent = floor(tmpindent);
                if  (newindent > currentindent) {
                    program = lexid_dynarray_add(program, NEWLINE_LEXID);
                    tmpid = BEGIN_LEXID;
                    for (i = 0; i < (newindent - currentindent - 1); i++) {
                        program = lexid_dynarray_add(program, BEGIN_LEXID);
                    }
                }
                else if (newindent < currentindent) {
                    program = lexid_dynarray_add(program, NEWLINE_LEXID);
                    tmpid = END_LEXID;
                    for (i = 0; i < currentindent - newindent - 1; i++) {
                        program = lexid_dynarray_add(program, END_LEXID);
                    }
                } 
                currentindent = newindent;
                ungetc(current, file); 
                break;
            case '0': case '1': case '2': case '3': case '4':
            case '5': case '6': case '7': case '8': case '9':
                tmpid = lexNum(current, 0, file);
                break;
            case '"':
                tmpid = lexString(current, file);
                break;
            case '-':
                //We want to be able to handle subtraction AND negative numbers
                current = getc(file);
                if (isdigit(current)) {
                    tmpid = lexNum(current, 1, file);
                    break;
                }
                //note: no break! If it's not a number, we just continue to the default case
                ungetc(current, file);

            default:
                tmpid = lexIdentifier(current, symtable, &newlex, file);
                printf("%s", "ID");
                break;  
        }
        //add the lexed token to the program, and get the next character
        program = lexid_dynarray_add(program, tmpid);
        current = getc(file);
    }
    lex_result result;
    result.program = program;
    return result;
}
