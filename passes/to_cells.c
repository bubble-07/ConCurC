#include "../prims/function.h"
#include "to_cells.h"
#include "../prims/env.h"
#include "../libs/memoryman.h"
#include "../prims/global_function_table.h"

//Returns 1 if the given expression is a function definition
//Assumes in.data is "EXPR_LEXID"
int is_function_def(lexid_tree in) {
    //If we know it's a definition of some sort
    if (lexid_eq(in.children.begin[0].data, DEF_LEXID)) {
        //If the second argument to def is an expression [not just a name]
        if (lexid_eq(in.children.begin[1].data, EXPR_LEXID)) {
            //We're going to assume that it's a function.
            return 1;
        }
    }
    return 0;
}

/*

//Takes a lexid_tree and a given environment, and converts it to a cell_tree
//(recursive procedure)
cell_tree convert_to_cells(lexid_tree in, env environ) {

    //If the tree we recieve has no children, must be a leaf
    if (in.children.size == 0) {

        cell result; //This will be used to store the resulting cell

        //Since it's a leaf, there are only a few things it could be
        if (lexid_eq(in.data, INT_LEXID)) {
            //Must be an integer, so make an integer cell
            result = make_int_cell(in.data.attr.intval);
        }
        if (lexid_eq(in.data, FLOAT_LEXID)) {
            result = make_float_cell(in.data.attr.floatval);
        }
        if (lexid_eq(in.data, STRING_LEXID)) {
            result = make_string_cell(in.data.attr.stringval);
        }

        if (isNonPrimID(in.data)) {
            //Our input expression must be some kind of identifier,
            //Variable, or function name
            //We look it up in the environment first, then in the function table
            parameter_ptr envlookup = env_lookup(environ, in.data);
            if (!parameter_ptr_eq(result, parameter_ptr_lookup_failure)) {
                //We must have found our parameter in the environment!
                result = make_parameter_cell(envlookup);
            }
            else {
                //It's not in the environment -- look in the global function table


            }




}
*/

//Takes something of the form "name" or "(type name)",
//and if a type was specified, returns the type, but if
//it wasn't, return an unknown type
TypeInfo name_decl_to_type(lexid_tree in, string_dynarray backsymtable) {
    //TODO: fail gracefully in the case of a lookup failure
    if (lexid_eq(in.data, EXPR_LEXID)) {
        //type must've been specified
        lexid type_lexid = in.children.begin[0].data;
        //Look up the type corresponding to the lexid
        TypeRef result = get_TypeRef(backsymtable.begin[type_lexid.tokenval]);
        //Finalize and return
        return make_known_type(result);
    }
    else {
        //Otherwise, the type must still be unknown
        return make_unknown_type();
    }
}

//Gets the name associated with (type name) or name
lexid name_decl_to_name(lexid_tree in) {
    if (lexid_eq(in.data, EXPR_LEXID)) {
        //must be of format (type name)
        return in.children.begin[1].data;
    }
    //otherwise, must be right there
    return in.data;
}

parameter name_decl_to_param(lexid_tree in, string_dynarray backsymtable) {
    parameter result;
    result.name = name_decl_to_name(in);
    result.type = name_decl_to_type(in, backsymtable);
    return result;
}




//Returns a function generated from a top-level definition representing one
//[expression of format def ((type function) (type arg1)...) (body)
//or of format def (function arg1 arg2) (body)
//Assumption: the argument passed is already looking like a valid function.
function load_function_def(lexid_tree in, string_dynarray backsymtable) {
    function result;
    
    //Gets the part of the definition corresponding to the definition
    //of the function's name, return type, arguments, and argument types.
    lexid_tree_dynarray typeline = in.children.begin[1].children;

    //Assign the function's return type
    result.retType = name_decl_to_type(typeline.begin[0], backsymtable);

    //All that remains are the parameters
    result.params = parameter_dynarray_make(1);
    int i;
    //for each parameter
    for (i = 1; i < typeline.size; i++) {
        //Convert the current name/type declaration to a parameter
        parameter current = name_decl_to_param(typeline.begin[i], backsymtable);
        //Add it to the function's parameter list
        result.params = parameter_dynarray_add(result.params, current);
    }

    //Set the body of the function to the result of converting it to cells
    //With the environment given by the parameters of the function.
    //result.body = convert_to_cells(in.children.begin[2], params_to_env(result.params));
    
    //We're done
    return result;
}

//Create a simple wrapper around what we previously defined in collectnames
//to get the name (lexid) of a defined function.
lexid load_function_name(lexid_tree def) {
    return get_name_from_def(def.children);
}

//For now, just print out every function we make
//Do NOTHING else
//Assumes the file contains ONLY function definitions
void to_cells(parse_result in) {
    int i;
    //For every top-level definition
    for (i = 0; i < in.AST.children.size; i++) {
        //Convert it to a function
        function current = load_function_def(in.AST.children.begin[i], in.backsymtable);
        lexid name = load_function_name(in.AST.children.begin[i]);
        //Add it to the global function table
        global_table = add_function(global_table, name, current);
        printf("\nFunction definition: ");
        printf(to_cstring(in.backsymtable.begin[name.tokenval]));
        printf(" ");
        //Print it
        print_function(current, in.backsymtable);
    }
    return;
}

