#include "collectnames.h"

//Takes a definition AST expression and returns the name the definition maps 
//Need to be sure that the definition in question is a function.
lexid get_name_from_def(lexid_tree_dynarray def_expr) {
    //Note: Assumes def_expr.begin[0].data contains DEF_LEXID
    
    //Define the "typing expression" part to be the expression after "DEF"
    lexid_tree_dynarray typing_expr = def_expr.begin[1].children;

    if (!lexid_eq(typing_expr.begin[0].data, EXPR_LEXID)) {
        //The name must be right there [no declared return type]
        return typing_expr.begin[0].data;
    }
    //Otherwise, it must be "function returntype"
    lexid_tree_dynarray fun_ret_pair = typing_expr.begin[0].children;
    return fun_ret_pair.begin[1].data;
}

//Collect every function that's defined with "DEF"
void populate_global_table(lexid_tree in) {
    //If the root isn't an expression
    if (!lexid_eq(in.data, EXPR_LEXID)) {
       //TODO: throw an error [not a valid program] 
       return;
    }

    //For convenience, denote the upper-level of expressions as "defs"
    lexid_tree_dynarray defs = in.children;

    int i;
    //For every upper-level definition
    for (i=0; i < defs.size; i++) {

        //If our upper-level definition isn't some kind of expression
        if (!lexid_eq(defs.begin[i].data, EXPR_LEXID) || defs.begin[i].children.size < 1) {
            //TODO: Throw some kind of error [this can't happen in any valid program]
            return;
        }
        
        //Gets the expression that should begin with DEF
        lexid_tree_dynarray def_expr = defs.begin[i].children;

        if (lexid_eq(def_expr.begin[0].data, DEF_LEXID)) {
            //We must have some kind of definition!
            if (lexid_eq(def_expr.begin[1].data, EXPR_LEXID)) {
                //We must have some kind of definition that isn't
                //A definition of a constant.
                lexid function_name = get_name_from_def(def_expr);

                //Add it to the global function table as an empty entry
                add_empty_polymorph(global_table, function_name);
            }
        }
    }
}
                
parse_result collectnames(parse_result in) {
    global_table = init_function_table(); //Initialize global table as empty
    populate_global_table(in.AST);
    return in; //Return the input, untouched
}

