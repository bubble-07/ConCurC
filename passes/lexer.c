#include "lexer.h"
#include "../libs/error.h"
#include "math.h"
//LEXER -- lex() gets you a stream of tokens. First, some helper functions.

int isNewline(char c) {
    return c == '\n' || c == '\r';
}


char get_char(fileLoc* file) {
    file->linepos += 1;
    char current = getc(file->fileptr);
    if (isNewline(current)) {
        file->lineno += 1;
        file->linepos = 0;
    }
    return current;
}
void unget_char(char current, fileLoc* file) {
    ungetc(current, file->fileptr);
    file->linepos -= 1;
    if (isNewline(current)) {
        file->lineno -= 1;
        file->linepos = LINE_END;
    }
    return;
}

//takes the current character and whether it's supposed to be negative or not
//returns a lexid that represents either a float or an int
lexid lexNum(char current, int negative, fileLoc* file) {
    lexid tmpid = INT_LEXID;
    unget_char(current, file);
    fscanf(file->fileptr, "%d", &tmpid.attr.intval);
    current = get_char(file);
    if (current != '.') {
        unget_char(current, file);
        if (negative) {
            tmpid.attr.intval = -tmpid.attr.intval;
        }
    }
    else {
        int afterdecimal;
        double nonintegral;
        fscanf(file->fileptr, "%d", &afterdecimal);
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
    return isNotGlobTerm(c) && c != '(' && c != ' ' && c != ')' && c != ',' && c != '\t' &&
        c != '.';
}

//takes the current character, a symbol table, and a reference to the new unique identifier num
//returns a lexid with the new identifier (if not seen before) or a lexid of an old one.
lexid lexIdentifier(char current, string_lexid_dict symtable, int* newlex, fileLoc* file) {
    lexid tmpid;
    char_dynarray id = to_dynstring("");
    while (isNotTerm(current)) {    
        id = char_dynarray_add(id, current);
        current = get_char(file);
    }
    unget_char(current, file);
    lexid lookupval = string_lexid_dict_get(symtable, id);
    if (lexid_eq(lookupval, lexid_lookup_failure)) {
        tmpid.tokenval = *newlex;
        tmpid.attr.intval = 0;
        symtable = string_lexid_dict_add(symtable, string_lexid_bucket_make(id, tmpid));
        lookupval = tmpid;
        *newlex = *newlex + 1;
    }
    else {
        char_dynarray_free(id);
    }
    return lookupval;
}

//Lexes a string literal
lexid lexString(char current, fileLoc* file) {
    lexid tmpid = STRING_LEXID;
    char_dynarray stringval = to_dynstring("");
    current = get_char(file);
    while (current != '"') {
        stringval = char_dynarray_add(stringval, current);
        current = get_char(file);
    }
    tmpid.attr.stringval = stringval;
    return tmpid;
}

//convenience macro to fill the symtable with core forms [primitives] so their token values
//are predictable
#define ADDCONST_SYM(nstring, constid) symtable = \
string_lexid_dict_add(symtable, string_lexid_bucket_make(to_dynstring(nstring), constid))


lex_result lex(fileLoc* currentloc) {
    int INDENT_TYPE = 0; //0 if not set, 1 if spaces for the file, 2 if tabs
                        //used later to throw an error for mixing tabs and spaces
    string_lexid_dict symtable = string_lexid_dict_init(100);
    lexid_dynarray program = lexid_dynarray_make(100);
    
    int repl = 0; //variable that is 1 if file is stdin, else 0
    if (currentloc->fileptr == stdin) {
        repl = 1;
    }

    /*filling the symtable with core forms*/
    ADDCONST_SYM("def", DEF_LEXID);
    ADDCONST_SYM("lambda", LAMBDA_LEXID);
    ADDCONST_SYM("namespace", NAMESPACE_LEXID);
    ADDCONST_SYM("import", IMPORT_LEXID);
    ADDCONST_SYM("type", TYPE_LEXID);

    ADDCONST_SYM("<:", SUBS_LEXID); 
//    ADDCONST_SYM("subs", SUBS_LEXID);//TODO: Fix shadowing of those with same lexid
//    ADDCONST_SYM("subtypes", SUBS_LEXID);

//    ADDCONST_SYM("sups", SUPS_LEXID);
    ADDCONST_SYM(":>", SUPS_LEXID);
//    ADDCONST_SYM("supertypes", SUPS_LEXID);


    //Add constant symbols corresponding to core [hardcoded] types
    ADDCONST_SYM("Int", INTID_LEXID);
    ADDCONST_SYM("Num", NUMID_LEXID);
    ADDCONST_SYM("String", STRINGID_LEXID);
    ADDCONST_SYM("Float", FLOATID_LEXID);
    ADDCONST_SYM("Any", ANYID_LEXID);
    ADDCONST_SYM("Either", EITHERID_LEXID);

    /*Meat of the actual lexer. Conceptually, it does this by predictive decisions using the 
    current character as a reference.*/
    int i = 0;
    int currentindent = 0;
    int newlex = LAST_PRIM_ID_VAL + 1; //Start generating new entries after the last primitive
    char current = get_char(currentloc);

    while (isNotGlobTerm(current) || (!repl && isNewline(current))) {
        lexid tmpid;
        //Consume until we get something that isn't a space
        if (current == ' ') {
            tmpid = SPACE_LEXID;
            tmpid.loc = *currentloc;
            program = lexid_dynarray_add(program, tmpid);
            while (current == ' ') {
                current = get_char(currentloc);
            }
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
                tmpid = NEWLINE_LEXID;
                tmpid.loc = *currentloc;
                program = lexid_dynarray_add(program, tmpid);
                current = get_char(currentloc);
                float tmpindent;
                for (tmpindent = 0; 
                     current == ' ' || current == '\t'; current = get_char(currentloc)) {
                    if (current == '\t') {
                        if (INDENT_TYPE == 0) {
                            INDENT_TYPE = 1; //tabs
                        }
                        else if (INDENT_TYPE == 2) {
                            fatal_error("You awful person -- mixing tabs and spaces...");
                        }
                        tmpindent = tmpindent + 1.0000;
                    }
                    if (current == ' ') {
                        if (INDENT_TYPE == 0) {
                            INDENT_TYPE = 2; //spaces
                        }
                        else if (INDENT_TYPE == 1) {
                            fatal_error("You awful person -- mixing tabs and spaces...");
                        }
                        tmpindent = tmpindent + 0.2500;
                    }
                }
                int newindent = floor(tmpindent);

                if  (newindent > currentindent) {
                    for (i = 0; i < (newindent - currentindent); i++) {
                        tmpid = BEGIN_LEXID;
                        tmpid.loc = *currentloc;
                        program = lexid_dynarray_add(program, tmpid);
                    }
                }
                else if (newindent < currentindent) {
                    for (i = 0; i < currentindent - newindent; i++) {
                        tmpid = END_LEXID;
                        tmpid.loc = *currentloc;
                        program = lexid_dynarray_add(program, tmpid);
                    }
                } 
                tmpid = NONE_LEXID;
                currentindent = newindent;
                unget_char(current, currentloc); 
                break;
            case '0': case '1': case '2': case '3': case '4':
            case '5': case '6': case '7': case '8': case '9':
                tmpid = lexNum(current, 0, currentloc);
                break;
            case '"':
                tmpid = lexString(current, currentloc);
                break;
            case '-': case '/':
                //We want to be able to handle comments AND
                //We want to be able to handle subtraction AND negative numbers
                if (current == '-') {
                    current = get_char(currentloc);
                    if (isdigit(current)) {
                        tmpid = lexNum(current, 1, currentloc);
                        break;
                    }
                    //note: no break! If it's not a number, we just continue to the default case
                    unget_char(current, currentloc);
                    current = '-';
                }
                else {
                    current = get_char(currentloc);
                    if (current == '/') {
                        while (current != '\n') {
                            current = get_char(currentloc);
                        }
                        tmpid = NEWLINE_LEXID;
                        break;
                    }
                    else if (current == '*') {
                        int done = 0;
                        while (!done) {
                            current = get_char(currentloc);
                            if (current == '*') {
                                current = get_char(currentloc);
                                if (current == '/') {
                                    done = 1;
                                }
                                else {
                                    unget_char(current, currentloc);
                                }
                            }
                        }
                        current = get_char(currentloc);
                        while (current == ' ') {
                            current = get_char(currentloc);
                        }
                        unget_char(current, currentloc);
                        tmpid = NONE_LEXID;
                        break;
                    }
                    unget_char(current, currentloc);
                    current = '/';
                }

            default:
                tmpid = lexIdentifier(current, symtable, &newlex, currentloc);
                break;  
        }
       
        tmpid.loc = *currentloc;

        //add the lexed token to the program, and get the next character
        if (tmpid.tokenval != 0) {
            program = lexid_dynarray_add(program, tmpid);
        }
        current = get_char(currentloc);
    }
    lex_result result;
    result.program = program;
    result.file = currentloc->file;

    //Invert our symtable so we can look up strings later.
    result.names = nametable_make(symtable, newlex + 1);

    string_lexid_dict_free(symtable);

    return result;
}
