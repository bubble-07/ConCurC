#include "lexer.h"
#include "tree.h"

#ifndef PARSERDEFINED
#define PARSERDEFINED

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

parser_state parser_state_init(lexid_dynarray prog, size_t i) {
    parser_state result;
    result.program = prog;
    result.index = i;
    return result;
}

lexid parser_state_getRel(parser_state in, size_t offset) {
    return in.program.begin[in.index + offset];
}


parser_state consume(lexid toconsume, parser_state in) {
    if (lexid_eq(toconsume, in.program.begin[in.index])) {
       return parser_state_init(in.program, in.index + 1);
    }
    return in; //TODO: throw an error!
}




parse_part parse_list(parser_state state) {
     parse_part result;
     result.tree = lexid_tree_init(EXPR_LEXID);
     state = consume(LPAREN_LEXID, state);
     lexid current;
     parse_part subexpr;
     size_t i = state.index;
     while (i < state.program.size && state.program.begin[i].tokenval != RPAREN) {
         current = state.program.begin[i];
         switch (current.tokenval) {
             case LPAREN:
                subexpr = parse_list(parser_state_init(state.program, i));
                result.tree = lexid_tree_addchild(result.tree, subexpr.tree);
                i = subexpr.state.index;
                break;
             default:
                result.tree = lexid_tree_addchild(result.tree, lexid_tree_init(current));
                break;
         }
         i++;
      }
      result.state.index = i;
      return result;
}

parse_result parse(lex_result in) {
     parse_result result;
     result.backsymtable = in.backsymtable;
     result.AST = parse_list(parser_state_init(in.program, 0)).tree;
     return result;
}

#endif
