#include "lexer.h"
#include "../libs/tree.h"

#ifndef PARSERDEFINED
#define PARSERDEFINED

/*The parser takes the token stream from lexing and turns it into an abstract syntax tree
of lexids, where EXPR_LEXID as the root data represents that the children form an expression*/

DEFINE_TREE(lexid)

typedef struct {
    lexid_tree AST;
    nametable names;
    string file;
} parse_result;

/*a simple handle to the parser state [program, current index] -- used to pass around the current
position in the token stream*/
typedef struct {
    lexid_dynarray program;
    size_t index;
} parser_state;

/*a partial result of a parse, containing the tree so far and the current state*/
typedef struct {
    lexid_tree tree;
    parser_state state;
} parse_part;

/*Explained in parser.c */
parser_state parser_state_init(lexid_dynarray, size_t i);

parser_state consume(lexid toconsume, parser_state in);

parse_part parse_listitem(parser_state state);
parse_part parse_dotitem(parser_state state);
parse_part parse_dotapp(parser_state state);
parse_part parse_sexpr(parser_state state);
//singleln true if on single line, false if not
parse_part parse_listitems(parser_state state, int singleln);
parse_part parse_funapp(parser_state state);
parse_part parse_blockline(parser_state state);
parse_part parse_blocklines(parser_state state);

/*The actual parsing function */
parse_result parse(lex_result in);

#endif
