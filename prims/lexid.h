//Defines what a lexid is

#include "../libs/filehandler.h"
#include "../libs/dynstring.h"

#ifndef LEXID_DEFINED
#define LEXID_DEFINED

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

/*defines common lex identifier constants, to make it easier to declare them in code*/
#define D_LEX(name, value) const static int name = value; \
                           const static lexid name##_LEXID = {value, 0};

D_LEX(NONE, 0)
D_LEX(LPAREN, 1)
D_LEX(RPAREN, 2)
D_LEX(OUTOFFILE, 14)    

D_LEX(DOT, 3)
D_LEX(BEGIN, 4)
D_LEX(END, 5)
D_LEX(NEWLINE, 6)
D_LEX(COMMA, 7)
D_LEX(SPACE, 8)

D_LEX(STRING, 10)
D_LEX(FLOAT, 11)
D_LEX(INT, 12)
D_LEX(FILEREF, 13)
D_LEX(DEF, 21)
D_LEX(LAMBDA, 22)
D_LEX(NAMESPACE, 23)
D_LEX(IMPORT, 24)
D_LEX(TYPE, 25)
D_LEX(SUBS, 26)
D_LEX(SUPS, 27)
D_LEX(EXPR, 28) //Somewhat special -- see NOTE below

//Define some core type lexids to get ourselves started
D_LEX(INTID, 29)
D_LEX(NUMID, 30)
D_LEX(STRINGID, 31)
D_LEX(FLOATID, 32)
D_LEX(ANYID, 33)
D_LEX(EITHERID, 34)
D_LEX(LISTID, 35)
D_LEX(NONEID, 36) 


//Set the value of the last primitive lexid (so all non-hardcoded identifiers come after)
#define LAST_PRIM_ID_VAL 36

//Necessary to allow lookup failure in a dictionary. This is why LPAREN is 1, not 0*/
const static lexid lexid_lookup_failure = {0, 0};

//NOTE: spooky action-at a distance that lets us compare lexid_eq values for in dicts
inline static int lexid_eq(lexid one, lexid two) {
    if (one.tokenval == two.tokenval) {
        return 1;
    }
    return 0;
}

//NOTE: EXPR_LEXID denotes an __expression__ -- it has an attribute that takes on
//an integer value, which is 1 if the expression has a well-defined function location
//(termed "strict" ordering) and 0 if the expression can be reordered.
//Example of a non-strict expression: (1 + 2) example of a strict one: +(1,2)

//Returns 1 if the passed EXPR_LEXID is strict, 0 otherwise
inline static int EXPR_LEXID_strict(lexid in) {
    return in.attr.intval;
}


inline static void lexid_free(lexid in) {
    if (lexid_eq(in, STRING_LEXID) | lexid_eq(in, FILEREF_LEXID)) {
        char_dynarray_free(in.attr.stringval);
        return;
    }
    return;
}

inline static int hash_lexid(lexid in) {
    return in.tokenval;
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

DEFINE_DYNARRAY(lexid)

#endif
