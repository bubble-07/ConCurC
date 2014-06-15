#include "typeinfer.h"

//Given a reference to an expression involving a function application, returns the respective
//types of arguments
type_ref_dynarray get_arg_type_refs(cell_tree in) {
    type_ref_dynarray result = type_ref_dynarray_make(1);
    int i;
    for (i=1; i < cell_tree_numchildren(in); i++) {
        result = type_ref_dynarray_add(result, get_cell_type_ref(cell_tree_child_data(in, i)));
    }
    return result;
}

//Recursive procedure that produces type equations given a cell tree
type_equation_dynarray gen_type_equations(cell_tree in, type_equation_dynarray eqns) {
    //Do everything depth-first
    size_t i;
    for (i=0; i < cell_tree_numchildren(in); i++) {
        eqns = gen_type_equations(cell_tree_child(in, i), eqns); //Add equations of subtrees
    }

    //Now, handle the current node
    CellType kind = cell_tree_getkind(in);
    cell node = cell_tree_data(in);
    type_ref node_type_ref = cell_tree_get_type_ref(in);
    
    //If we're dealing with a polymorph in the applicative position
    if (kind == POLYMORPH && cell_tree_isapplicative(in)) {
        //Get the type refs of the arguments
        type_ref_dynarray argtypes = get_arg_type_refs(cell_tree_parent(in));
        //Get the polymorph referenced by the node
        polymorph_ptr poly = node.data;
        //Generate the type equation
        type_equation poly_eqn = make_is_polymorph_equation(node_type_ref, poly, argtypes);
        //Add it
        eqns = type_equation_dynarray_add(eqns, poly_eqn);
    }

    //If we're dealing with a compound expression
    if (kind == EXPRCELL) {
        type_ref_dynarray argtypes = get_arg_type_refs(in);
        type_ref functype = get_cell_type_ref(cell_tree_child_data(in, 0));
        type_equation expr_eqn = make_is_result_of_equation(node_type_ref, functype, argtypes);
        //Add it
        eqns = type_equation_dynarray_add(eqns, expr_eqn);
    }

    //If we're dealing with a parameter of some kind
    if (kind == PARAMETER) {
        //Need to replace the cell's type ref with the parameter's, then continue

        //TODO: Memory management!
        parameter_ptr p = node.data;
        type_ref paramtype = p->type;
        cell_tree_set_type_ref(in, paramtype);
    }


    //If we're not in the applicative position, will need to emit an argpos equation
    if (!cell_tree_isapplicative(in)) {
        int pos = cell_tree_get_offset(in);
        //Get the type ref to the function we're dealing with
        type_ref func = get_cell_type_ref(cell_tree_child_data(cell_tree_parent(in), 0));
        //Generate the equation
        type_equation argpos_eqn = make_is_in_pos_equation(node_type_ref, pos, func);
        eqns = type_equation_dynarray_add(eqns, argpos_eqn);
    }

    return eqns;
}


//Infers types in a given function body
cell_tree infer_body(cell_tree in) {
    type_equation_dynarray equations = gen_type_equations(in, type_equation_dynarray_make(1)); //Generates type equations from the tree
    //Print them out (debugging)
    print_type_equations(equations);
    //equations = solve_type_equations(equations); //Solves type equations
    //in = elaborate_types(in, equations); //Pass to plug in the simplified equations to the tree
    return in;
}


//Infers argument types, return types, and body types of given function_ptr
function_ptr infer_function(function_ptr in) {
    //Nothing to be done for parameters, since only meaningful references are in the body

    //Infers types for the body of the function
    in->body = infer_body(in->body);
    
    //Set the return type of the function to the outermost body expression type
    cell outermost = cell_tree_data(in->body);
    in->ret_type = outermost.type;

    return in;
}


//Infers all types associated with the given polymorph
polymorph_ptr infer_polymorph(polymorph_ptr in) {
    //Run inference on all functions associated with the polymorph
    in->options = function_ptr_dynarray_map(in->options, &infer_function);
    return in;
}


//For now, just type infer sequentially
//TODO: Change the order to be dependency-based!

def_collection typeinfer(def_collection in) {
    in.funcs = polymorph_ptr_dynarray_map(in.funcs, &infer_polymorph);
    return in;
}

