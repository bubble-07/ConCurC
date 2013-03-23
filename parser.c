#include "parser.h"

/*initializes a parser state from a token stream and an index*/
parser_state parser_state_init(lexid_dynarray prog, size_t i) {
    parser_state result;
    result.program = prog;
    result.index = i;
    return result;
}

int boundsCheck(parser_state state) {
    if (state.index < state.program.size && state.index) {
        return 1;
    }
    return 0;
}
lexid getCurrent(parser_state state) {
    if (!boundsCheck(state)) {
        printf("%s", "ERRRRRORRRR");
    }    
    return state.program.begin[state.index];
}


/*consumes one token from the stream*/
parser_state consume(lexid toconsume, parser_state in) {
    if (lexid_eq(toconsume, getCurrent(in))) {
       return parser_state_init(in.program, in.index + 1);
    }
    return in; //TODO: throw an error!
}

parse_part parse_listitem(parser_state state) {
    parse_part result;
    lexid current = getCurrent(state);
    if (lexid_eq(FLOAT_LEXID, current) || lexid_eq(INT_LEXID, current)
        || lexid_eq(STRING_LEXID, current)) {

        result.tree = lexid_tree_init(current);
        result.state = state;
        result.state.index += 1;
    }
    else if (lexid_eq(LPAREN_LEXID, current)) {
        result = parse_sexpr(state);
    }
    else if ( isID(current) ) {
        result = parse_dotapp(state);
    }
    else {
        printf("%s", "ERROR!");
    }
    return result; 
}

parse_part parse_dotitem(parser_state state) {
    parse_part result;
    state.index += 1;
    lexid current = getCurrent(state);
    state.index -= 1;
    if (lexid_eq(LPAREN_LEXID, current)) {
        result = parse_funapp(state);
    }
    else {
        result.tree = lexid_tree_init(getCurrent(state));
        result.state = state;
        result.state.index += 1;
    }
    return result;
}

parse_part parse_funapp(parser_state state) {
    parse_part result;
    parse_part tmp;
    lexid exprid = EXPR_LEXID;
    exprid.loc = getCurrent(state).loc;
    result.tree = lexid_tree_init(exprid);
    result.tree = lexid_tree_addchild(result.tree, lexid_tree_init(getCurrent(state)));
    //get us past the opening paren
    state.index += 2;
    while (!lexid_eq(getCurrent(state), RPAREN_LEXID)) {
        tmp = parse_listitems(state,0);
        //flatten if there's just one element
        if (tmp.tree.children.size == 1) {
            result.tree = lexid_tree_addchild(result.tree, tmp.tree.children.begin[0]);
            lexid_tree_free(tmp.tree);
        } 
        else {
            result.tree = lexid_tree_addchild(result.tree, tmp.tree);
        }
        state = tmp.state;
        if (lexid_eq(getCurrent(state), COMMA_LEXID)) {
            state = consume(COMMA_LEXID, state);
        }
    }
    state = consume(RPAREN_LEXID, state);
    result.state = state;
    return result;
}

parse_part parse_dotapp(parser_state state) {
    parse_part result;
    parse_part temp;
   
    lexid exprid = EXPR_LEXID;
    exprid.loc = getCurrent(state).loc;

    lexid_tree_dynarray dotsepvals = lexid_tree_dynarray_make(5); 
    lexid current = DOT_LEXID;
    while (lexid_eq(current, DOT_LEXID)) {
        temp = parse_dotitem(state);
        dotsepvals = lexid_tree_dynarray_add(dotsepvals, temp.tree);
        state = temp.state;
        current = getCurrent(state);
        state.index += 1;
    }
    state.index -= 1;

    result.state = state;

    if (dotsepvals.size == 1) {
        result.tree = dotsepvals.begin[0];
        lexid_tree_dynarray_free(dotsepvals);
        return result;
    }
    
    int i;
    lexid_tree tmp;
    result.tree = lexid_tree_init(exprid);
    result.tree = lexid_tree_addchild(result.tree, dotsepvals.begin[1]);
    result.tree = lexid_tree_addchild(result.tree, dotsepvals.begin[0]);
    for (i = 2; i < dotsepvals.size; i++) {
        tmp = result.tree;
        result.tree = lexid_tree_addchild(lexid_tree_init(exprid), dotsepvals.begin[i]);
        result.tree = lexid_tree_addchild(result.tree, tmp);
    }
    lexid_tree_dynarray_free(dotsepvals);
    return result;
    
}

parse_part parse_sexpr(parser_state state) {
    parse_part result;
    state = consume(LPAREN_LEXID, state);
    result = parse_listitems(state,0);
    result.state = consume(RPAREN_LEXID, result.state);
    return result;
}
 
/*Parses list items out of the token stream. Takes the current parser state, and returns a partial
result*/
//if singleline is non-zero, then parsing is for a single line
parse_part parse_listitems(parser_state state, int singleln) {
    parse_part result;
    parse_part tmp;

    state.index -= 1;

    lexid exprid = EXPR_LEXID;
    if (boundsCheck(state)) {
        exprid.loc = getCurrent(state).loc;
    }

    result.tree = lexid_tree_init(exprid);
    
    lexid current = SPACE_LEXID;

    while (singleln ? lexid_eq(current, SPACE_LEXID) : isWhite(current)) {
        while (singleln ? lexid_eq(current, SPACE_LEXID) : isWhite(current)) {
            state.index += 1;
            current = getCurrent(state);
        }
        if (singleln && isWhite(current)) {
            continue;
        }
        if (lexid_eq(current, COMMA_LEXID) || lexid_eq(current, RPAREN_LEXID)) {
            continue;
        }
        tmp = parse_listitem(state);
        result.tree = lexid_tree_addchild(result.tree, tmp.tree);
        state.index = tmp.state.index;
        current = getCurrent(state);
    }
    result.state = state;
    return result;
}

parse_part parse_blockline(parser_state state) {
    parse_part result;
    
    //for the block body formatting
    lexid_tree header;
    lexid_tree tmp;
    int i;

    lexid exprid = EXPR_LEXID;
    exprid.loc = getCurrent(state).loc;

    result = parse_listitems(state,1);
    result.state = consume(NEWLINE_LEXID, result.state);
    //may have to protect against "end of program" bugs...
    if (!boundsCheck(result.state)) {
        return result;
    }
    if (lexid_eq(getCurrent(result.state),BEGIN_LEXID)) {
        //parse block body
        header = result.tree;
        result.state = consume(BEGIN_LEXID, result.state);
        result = parse_blocklines(result.state);
        result.state = consume(END_LEXID, result.state);

        //assemble the formatting correctly
        //Format the header
        tmp = lexid_tree_init(exprid);
        for (i=1; i < header.children.size; i++) {
            tmp = lexid_tree_addchild(tmp, header.children.begin[i]);
        }
        header = lexid_tree_addchild(lexid_tree_init(exprid), header.children.begin[0]);
        header = lexid_tree_addchild(header, tmp);

        //Assimilate with the body
        tmp.children = result.tree.children;
        result.tree.children = lexid_tree_dynarray_cat(header.children, tmp.children);
        lexid_tree_dynarray_free(tmp.children);
        
    }
    //parsed a block line, we're good, return
    return result; 
}

//parse blocklines until we hit an "end"
parse_part parse_blocklines(parser_state state) {
    parse_part result;
    parse_part tmp;

    lexid exprid = EXPR_LEXID;
    exprid.loc = getCurrent(state).loc;

    result.tree = lexid_tree_init(exprid);
    while ( state.index < state.program.size ? !lexid_eq(getCurrent(state), END_LEXID) : 0) {
        tmp = parse_blockline(state);
        if (tmp.tree.children.size == 1) {
            tmp.tree = tmp.tree.children.begin[0];
            result.tree = lexid_tree_addchild(result.tree, tmp.tree);
        }
        else if (tmp.tree.children.size == 0) {
        }
        else {
            result.tree = lexid_tree_addchild(result.tree, tmp.tree);
        }
        state = tmp.state;
    }
    result.state = state;
    return result;
}


/*Glue to make sure the types match up*/
parse_result parse(lex_result in) {
     parse_result result;
     result.backsymtable = in.backsymtable;
     result.AST = parse_blocklines(parser_state_init(in.program, 0)).tree;
     result.file = in.file;
     lexid_dynarray_free(in.program);
     return result;
}
