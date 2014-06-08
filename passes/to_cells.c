#include "to_cells.h"
#include "../prims/env.h"
#include "../libs/memoryman.h"
#include "../prims/function_table.h"

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


//Takes something of the form "name" or "(type name)",
//and if a type was specified, returns the type, but if
//it wasn't, return an unknown type
TypeInfo name_decl_to_type(lexid_tree in) {
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

parameter name_decl_to_param(lexid_tree in) {
    parameter result;
    result.name = name_decl_to_name(in);
    result.type = name_decl_to_type(in);
    return result;
}


//Makes a lambda form cell_tree from the given exprs for params and body
//(Return type is left unspecified)
cell_tree make_lambda_expr(lexid_tree paramtree, lexid_tree body, env e, function_table table) {
    lambda* head = memalloc(sizeof(lambda));
    head->params = parameter_dynarray_make(1);

    if (!lexid_eq(paramtree.data, EXPR_LEXID)) {
        //Must be a single parameter without a type given (right there)
        head->params = parameter_dynarray_add(head->params, name_decl_to_param(paramtree));
    }
    else {
        //Must have multiple parameters
        int i;
        for (i=0; i < paramtree.children.size; i++) {
            parameter current = name_decl_to_param(paramtree.children.begin[i]);
            head->params = parameter_dynarray_add(head->params, current);
        }
    }
    
    //Initialize the resulting tree with the lambda header we constructed
    cell_tree result = cell_tree_init(make_lambda_cell(head));

    //Fork off an environment from the one passed in to be used for converting the body
    env innerenv = fork_env(&e, head->params);

    //Add the converted body directly below the header
    result = cell_tree_addchild(result, convert_to_cells(body, innerenv, table));
    //Clean up the environment we created
    free_env(innerenv);

    return result;
}

//Converts the line of a lambda expression to a cell_tree
cell_tree convert_lambda_expr(lexid_tree_dynarray in, env e, function_table table) {
    cell_tree result;
    if (in.size == 3) {
        //Must not have specified a return type
        result = make_lambda_expr(in.begin[1], in.begin[2], e, table);
        //Get a pointer to the record for the lambda
        cell lambdahead = cell_tree_data(result);
        lambda_ptr ptr = lambdahead.data;
        ptr->retType = make_unknown_type();
        return result;
    }
    if (in.size == 4) {
        //Return type must be specified for the first arg
        result = make_lambda_expr(in.begin[2], in.begin[3], e, table);
        cell lambdahead = cell_tree_data(result);
        lambda_ptr ptr = lambdahead.data;
        ptr->retType = make_known_type(get_TypeRef(in.begin[1].data));
        return result;
    }
    return result; //TODO: THROW AN ERROR! (must be malformed)
}


//Converts a leaf lexid to a cell_tree
cell_tree convert_to_singleton_cell(lexid in, env e, function_table table) {

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
            //It's not in the environment -- look in the polymorph table
            if (has_polymorph(table, in)) {
                //Great, it registers with some known polymorph
                result = make_polymorph_cell(get_polymorph_ptr(table, in));
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
cell_tree convert_to_cells(lexid_tree in, env e, function_table table) {

    //If the tree we recieve has no children, must be a leaf
    if (in.children.size == 0) {
        //Return our leaf node
        return convert_to_singleton_cell(in.data, e, table);
    }
    //Otherwise....
    //The tree we recieved must be an expression, since it has children.
    if (!lexid_eq(in.data, EXPR_LEXID)) {
        //TODO: Throw a nicer error, and actually abort nicely!
        printf("\n ERROR! Expected an expression \n");
    }

    //Handle the "lambda" special form
    if (lexid_eq(in.children.begin[0].data, LAMBDA_LEXID)) {
        return convert_lambda_expr(in.children, e, table);
    }

    //Must be a simple expression, so make an expression subtree
    cell head = make_expr_cell();
    head.loc = in.data.loc; //Set the file location accordingly
    cell_tree result = cell_tree_init(head);
    
    //recursively convert children
    int i;
    for (i = 0; i < in.children.size; i++) {
        //Add the results of converting each child under the same environment
        cell_tree converted = convert_to_cells(in.children.begin[i], e, table);
        result = cell_tree_addchild(result, converted);
    }
    //Not quite done yet -- if the original expression had non-strict ordering,
    //We need to check to see if we can move things to the beginning.

    if (!EXPR_LEXID_strict(in.data)) {
        //Loop through, and move the first callable cell to the beginning
        for (i=0; i < cell_tree_numchildren(result); i++) {
            if (cell_is_callable(cell_tree_child_data(result, i))) {
                //Move the first callable element to the applicative position
                result = cell_tree_make_child_applicative(result, i);
                //Return immediately
                return result;
            }
        }
    }

    return result;
}

//Create a simple wrapper around what we previously defined in collectnames
//to get the name (lexid) of a defined function.
lexid load_function_name(lexid_tree def) {
    return get_name_from_def(def.children);
}

//Returns a function generated from a top-level definition representing one
//[expression of format def ((type function) (type arg1)...) (body)
//or of format def (function arg1 arg2) (body)
//Assumption: the argument passed is already looking like a valid function.
function load_function_def(lexid_tree in, function_table table) {
    function result;
    
    //Gets the part of the definition corresponding to the definition
    //of the function's name, return type, arguments, and argument types.
    lexid_tree_dynarray typeline = in.children.begin[1].children;

    //Assign the function's return type
    result.retType = name_decl_to_type(typeline.begin[0]);

    //All that remains are the parameters
    result.params = parameter_dynarray_make(1);
    int i;
    //for each parameter
    for (i = 1; i < typeline.size; i++) {
        //Convert the current name/type declaration to a parameter
        parameter current = name_decl_to_param(typeline.begin[i]);
        //Add it to the function's parameter list
        result.params = parameter_dynarray_add(result.params, current);
    }

    //Set the body of the function to the result of converting it to cells
    //With the environment given by the parameters of the function.
    env innerenv = params_to_env(result.params);

    result.body = convert_to_cells(in.children.begin[2], innerenv, table);

    //Get the function's name
    lexid name = load_function_name(in);
    result.name = name;

    //Free the temporary environment we created
    free_env(innerenv);
    
    //We're done
    return result;
}

//For now, just print out every function we make
//Do NOTHING else
//Assumes the file contains ONLY function definitions
def_collection to_cells(collectnames_result in) {
    lexid_tree parsetree = in.parse.AST;
    function_table table = in.table;
    nametable names = in.parse.names;

    int i;
    printf("\n");
    //For every top-level definition
    for (i = 0; i < parsetree.children.size; i++) {
        //Convert it to a function
        function current = load_function_def(parsetree.children.begin[i], table);

        //Add it to the passed function table
        table = add_function(table, current.name, current);
        //Print it
        print_function(current, names);
        printf("\n");
    }

    def_collection result;
    result.funcs = get_all_polymorph_ptrs(table); //Flatten our dictionary, we don't need it anymore
    result.names = names;
    return result;
}

