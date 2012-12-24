#include "parser.h"

/*initializes a parser state from a token stream and an index*/
parser_state parser_state_init(lexid_dynarray prog, size_t i) {
    parser_state result;
    result.program = prog;
    result.index = i;
    return result;
}


/*consumes one token from the stream*/
parser_state consume(lexid toconsume, parser_state in) {
    if (lexid_eq(toconsume, in.program.begin[in.index])) {
       return parser_state_init(in.program, in.index + 1);
    }
    return in; //TODO: throw an error!
}

/*Parses a list out of the token stream. Takes the current parser state, and returns a partial
result*/
parse_part parse_list(parser_state state) {
     parse_part result;
     /*get ready to store an expression*/
     result.tree = lexid_tree_init(EXPR_LEXID);
     /*consume opening paren*/
     state = consume(LPAREN_LEXID, state);
     lexid current;
     parse_part subexpr;
     /*While we haven't hit the end of the input, and haven't hit a right paren, if we find
     a left paren, recursively spawn this function, and by default add the current lexid
     as a child to the tree that results from a call to this function*/
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

/*Glue to make sure the types match up*/
parse_result parse(lex_result in) {
     parse_result result;
     result.backsymtable = in.backsymtable;
     result.AST = parse_list(parser_state_init(in.program, 0)).tree;
     return result;
}
