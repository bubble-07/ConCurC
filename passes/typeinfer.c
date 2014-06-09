#include "typeinfer.h"

//Defines simple type inference (as carried out from the bottom up)

//Restricts the type of a given cell representing a parameter using
//the funcion-like object in the second argument, assuming that the
//parameter is in argument position pos. As a side-effect, updates
//the parameter referenced
cell restrict_by_application(cell in, cell_tree functiontree, int pos) {
    parameter* param = cell_get_parameter_ptr(in);
    //Gets the information about the type that's currently known
    TypeInfo prior = get_parameter_ptr_type(param);
    //If we're not dealing with some wacky compound function
    //TODO: handle wacky compound functions and lambdas
    if (cell_tree_isleaf(functiontree)) {
        cell functioncell = cell_tree_data(functiontree);
        //If we are dealing with a polymorph
        if (functioncell.kind == POLYMORPH) {
            polymorph_ptr f = cell_get_polymorph_ptr(functioncell);
            //Gets all possible types that could be in the position
            //TODO: is there an off-by-one error here?
            TypeInfo possibilities = polymorph_ptr_get_parameter_type(f, pos - 1);
            //Restrict our prior typeinfo to fall under possible types
            TypeInfo updated = restrict_sum(prior, possibilities);
            //Update param's stored type info to reflect the updated type
            set_parameter_ptr_type(param, updated);
            //The cell's stored type isn't updated [not an expression]
            return in;
        }
    }
    return in; //Fallback option
}


//Checks a singleton node (any leaf in the cell_tree)
cell check_singleton(cell_tree in) {
    cell current = cell_tree_data(in);
    if (cell_is_callable(current)) {
        //TODO: Do something to represent function signatures
        return current;
    }
    //Handle various literals
    switch (current.kind) {
        case INTCELL:
            //TODO: Represent integers
        case FLOATCELL:
            //TODO: Represent floats
        case STRINGCELL:
            //TODO: Represent strings
            return current;
    }
    //Handle inference on parameters
    if (current.kind == PARAMETER) {
        //Check to see if it's part of a compound expression
        if (cell_tree_isroot(in)) {
            //Must not be part of a compound expression
            return current; //Nothing to add to the type sig
        }
        else {
            cell_tree enclosing = cell_tree_parent(in);
            //If we're dealing with a __function application__
            if (cell_is_callable(cell_tree_data(cell_tree_child(enclosing, 0)))) {
                //Get the position of the argument within enclosing space
                int pos = cell_tree_get_offset(in);
                //Use the function to restrict acceptable types for the parameter
                return restrict_by_application(current, cell_tree_child(enclosing,0), pos);
            }
            //TODO: handle lists
        }
    }
    return current; //If nothing else, just return it
} 

//Checks an expression rooted at the passed node
//Returns the same root cell, modified with new type information
cell check_expression(cell_tree in) {
    //It's an expression, so the first argument represents some kind of polymorph
    //Technically, by this stage, since we go bottom-up, left-to-right, no checking of
    //arguments needs to be done -- all we need to do is insert the most general return type

    //If we don't have some weird empty expression [TODO: Handle as an empty list!]
    if (cell_tree_numchildren(in) > 0) {
        cell poly = cell_tree_child_data(in, 0);
        polymorph_ptr p = poly.data;
        //Update the expression's type with the polymorph's most general return type
        return update_cell_type(cell_tree_data(in), polymorph_ptr_get_return_type(p));
    }
    return cell_tree_data(in); //Safe fallthrough
}

//Helper function to be used in bottom-up traversal
//Argument is the level of the tree being examined.
//Returns the cell updated with type information.
cell check_recursive(cell_tree in) {
    if (cell_tree_isleaf(in)) {
        return check_singleton(in);
    }
    //Otherwise, we must be dealing with an expression or a lambda
    if (cell_tree_isexpr(in)) {
        //It's an expression
        return check_expression(in);
    }
    return cell_tree_data(in); //Reasonable fallback
}

void check_function(function_ptr in) {
    cell_tree body = in->body;
    //Check the body of the function in a bottom-up manner
    body = cell_tree_bottom_up_transform(body, &check_recursive);
    return;
}

//TODO: Have this check totality condition on types and notimplemented
void check_polymorph(polymorph_ptr in) {
    function_ptr_dynarray options = polymorph_get_options(*in);
    int i;
    for (i=0; i < options.size; i++) {
        check_function(options.begin[i]);
    }
    return;
}

//For now, take a parse_result so we can debug and print stuff
def_collection typeinfer(def_collection in) {
    //Need to typecheck and infer for every function definition
    polymorph_ptr_dynarray functions = in.funcs;
    int i;
    for (i=0; i < functions.size; i++) {
        check_polymorph(functions.begin[i]);
        print_polymorph_ptr(functions.begin[i], in.names);
    }
    return in; //Done checking
}


