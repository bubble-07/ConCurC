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

//cell_tree make_lambda_expr(lexid_tree params, lexid_tree body);

//Converts the line of a lambda expression to a cell_tree
//cell_tree convert_lambda_expr(lexid_tree_dynarray in, env e) {
 //   cell_tree result;
  //  if (in.size == 3) {
   //     //Must not have specified a return type
    //    result = make_lambda_expr(

//Converts a leaf lexid to a cell_tree
cell_tree convert_to_singleton_cell(lexid in, env e) {

    cell result; //This will be used to store the resulting cell

    //Since it's a leaf, there are only a few things it could be
    if (lexid_eq(in, INT_LEXID)) {
        //Must be an integer, so make an integer cell
        result = make_int_cell(in.attr.intval);
    }
    if (lexid_eq(in, FLOAT_LEXID)) {
        result = make_float_cell(in.attr.floatval);
    }
    if (lexid_eq(in, STRING_LEXID)) {
        result = make_string_cell(in.attr.stringval);
    }

    if (isNonPrimID(in)) {
        //Our input expression must be some kind of identifier,
        //Variable, or function name
        //We look it up in the environment first, then in the function table
        parameter_ptr envlookup = env_lookup(e, in);
        if (!parameter_ptr_eq(envlookup, parameter_ptr_lookup_failure)) {
            //We must have found our parameter in the environment!
            result = make_parameter_cell(envlookup);
        }
        else {
            //It's not in the environment -- look in the global polymorph table
            if (has_polymorph(global_table, in)) {
                //Great, it registers with some known polymorph
                result = make_polymorph_cell(get_polymorph_ptr(global_table, in));
            }
            else {
                //TODO: Throw an error saying the identifier isn't in scope
                printf("\n Error! Identifier not in scope. \n");
            }
        }
    }
    //Set the file location of the resulting cell to what we got in
    result.loc = in.loc;
    return cell_tree_init(result);
}


//Takes a lexid_tree and a given environment pointer, and converts it to a cell_tree
//(recursive procedure)
//NOTE: For fun, rename "e" to "environ", and see the wacky error gcc gives!
cell_tree convert_to_cells(lexid_tree in, env e) {

    //If the tree we recieve has no children, must be a leaf
    if (in.children.size == 0) {
        //Return our leaf node
        return convert_to_singleton_cell(in.data, e);
    }
    //Otherwise....
    //The tree we recieved must be an expression, since it has children.
    if (!lexid_eq(in.data, EXPR_LEXID)) {
        //TODO: Throw a nicer error, and actually abort nicely!
        printf("\n ERROR! Expected an expression \n");
    }

    //Handle the "lambda" special form
    if (lexid_eq(in.children.begin[0].data, LAMBDA_LEXID)) {
        //return convert_lambda_expr(in.children, e);
    }

    //Must be a simple expression, so make an expression subtree
    cell_tree result = cell_tree_init(make_expr_cell());
    result.data.loc = in.data.loc; //Set file location of expression
    
    //recursively convert children
    int i;
    for (i = 0; i < in.children.size; i++) {
        //Add the results of converting each child under the same environment
        cell_tree converted = convert_to_cells(in.children.begin[i], e);
        result = cell_tree_addchild(result, converted);
    }
    //Not quite done yet -- if the original expression had non-strict ordering,
    //We need to check to see if we can move things to the beginning.

    if (!EXPR_LEXID_strict(in.data)) {
        //Loop through, and move the first callable cell to the beginning
        for (i=0; i < result.children.size; i++) {
            if (cell_is_callable(result.children.begin[i].data)) {
                //Move the first callable element to position 0
                result.children = cell_tree_dynarray_moveToBegin(i, result.children);
                //Return immediately
                return result;
            }
        }
    }

    return result;
}


//Takes something of the form "name" or "(type name)",
//and if a type was specified, returns the type, but if
//it wasn't, return an unknown type
TypeInfo name_decl_to_type(lexid_tree in, string_dynarray backsymtable) {
    //TODO: fail gracefully in the case of a lookup failure
    if (lexid_eq(in.data, EXPR_LEXID)) {
        //type must've been specified
        lexid type_lexid = in.children.begin[0].data;
        //Look up the type corresponding to the lexid
        TypeRef result = get_TypeRef(type_lexid);
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
    result.body = convert_to_cells(in.children.begin[2], params_to_env(result.params));
    
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

