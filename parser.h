#ifndef PARSERDEFINED
#define PARSERDEFINED

#include "lexer.h"
#include "libs/tree.h"


DEFINE_TREE(lexid)

typedef struct {
    lexid_tree AST;
    string_dynarray backsymtable;
} parse_result;


typedef struct {
    lexid_dynarray program;
    size_t index;
} parser_state;

typedef struct {
    lexid_tree tree;
    parser_state state;
} parse_part;

parser_state parser_state_init(lexid_dynarray, size_t i);

lexid parser_state_getRel(parser_state in, size_t offset);

parser_state consume(lexid toconsume, parser_state in);

parse_part parse_list(parser_state state);

parse_result parse(lex_result in);

#endif
