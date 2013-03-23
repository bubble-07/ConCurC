#ifndef LEXER_DEFINED
#define LEXER_DEFINED

#include <stdlib.h>
#include "libs/filehandler.h"
typedef union {
    double floatval;
    string stringval;
    int intval;
} lexattribute;

/*A lex identifier is a token val [indicating what kind of token it is] and an associated
attribute [if any], such as the value of a floating point literal. */

typedef struct {
    int tokenval;
    lexattribute attr;
    fileLoc loc;
} lexid;

/*macro definitions for the token values. EXPR is assumed to be last in the core token values.
User-defined identifiers continue after EXPR */
#define NONE 0
#define LPAREN 1
#define RPAREN 2
#define DOT 3
#define BEGIN 4
#define END 5
#define NEWLINE 6
#define COMMA 7
#define SPACE 8
#define STRING 10
#define FLOAT 11
#define INT 12
#define FILEREF 13
#define OUTOFFILE 14
#define DEF 21
#define LAMBDA 22
#define NAMESPACE 23
#define IMPORT 24
#define TYPE 25
#define SUBS 26
#define SUPS 27
#define EXPR 28

/*defines common lex identifier constants, to make it easier to declare them in code*/
#define D_LEX(name) const static lexid name##_LEXID = {name, 0};

D_LEX(NONE)
D_LEX(LPAREN)
D_LEX(RPAREN)
D_LEX(OUTOFFILE)    

D_LEX(DOT)
D_LEX(BEGIN)
D_LEX(END)
D_LEX(NEWLINE)
D_LEX(COMMA)
D_LEX(SPACE)

D_LEX(STRING)
D_LEX(FLOAT)
D_LEX(INT)
D_LEX(FILEREF)
D_LEX(DEF)
D_LEX(LAMBDA)
D_LEX(NAMESPACE)
D_LEX(IMPORT)
D_LEX(TYPE)
D_LEX(SUBS)
D_LEX(SUPS)
D_LEX(EXPR)

//Necessary to allow lookup failure in a dictionary. This is why LPAREN is 1, not 0*/
const static lexid lexid_lookup_failure = {0, 0};

//NOTE: spooky action-at a distance that lets us compare lexid_eq values for in dicts
inline static int lexid_eq(lexid one, lexid two) {
    if (one.tokenval == two.tokenval) {
        return 1;
    }
    return 0;
}

//Predicate that determines whether a given lexid is a primitive or not*/
inline static int isPrim(lexid in) {
    return lexid_eq(in, DEF_LEXID) || lexid_eq(in, LAMBDA_LEXID) || lexid_eq(in, SUPS_LEXID) 
    || lexid_eq(in, TYPE_LEXID) || lexid_eq(in, NAMESPACE_LEXID) || lexid_eq(in, SUBS_LEXID) 
    || lexid_eq(in, IMPORT_LEXID);
}

inline static int isID(lexid in) {
    return (isPrim(in) || (in.tokenval > ( EXPR )));
}
inline static int isNonPrimID(lexid in) {
    return (in.tokenval > EXPR);
}

inline static int isWhite(lexid in) {
    return (lexid_eq(in, SPACE_LEXID) || lexid_eq(in, BEGIN_LEXID) ||
            lexid_eq(in, END_LEXID) || lexid_eq(in, NEWLINE_LEXID));
}

/*defines a dictionary from strings to lexids in order to implement a working symbol table*/
DEFINE_DICT(string, lexid)


//LEXER: Input -- a string
//Output -- a dynamic array of lexids [program text] AND a dynarray of strings [symbols to names]
DEFINE_DYNARRAY(lexid)
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
