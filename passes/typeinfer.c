#include "typeinfer.h"
#include "../prims/type_ref_info.h"
#include "../prims/equation_set.h"

//For now, because we want to print things (but don't want to affect function signatures) make a global nametable
nametable names;

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
type_ref_table gen_type_equations(cell_tree in, type_ref_table table) {
    //Do everything depth-first and RIGHT TO LEFT (otherwise, polymorph eqns won't emit correctly)
    int i;
    for (i=cell_tree_numchildren(in) - 1; i >= 0; i--) {
        table = gen_type_equations(cell_tree_child(in, i), table); //Add equations of subtrees
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
        //Generate and add "polymorph" equation to the current cell
        node_type_ref = type_ref_addpoly_eqn(node_type_ref, poly, argtypes);
    }

    //If we're dealing with a compound expression
    if (kind == EXPRCELL) {
        type_ref_dynarray argtypes = get_arg_type_refs(in);
        type_ref functype = get_cell_type_ref(cell_tree_child_data(in, 0));
        //Generate and add "apply" equation
        node_type_ref = type_ref_addapply_eqn(node_type_ref, functype, argtypes);
    }

    //If we're dealing with a parameter of some kind
    if (kind == PARAMETER) {
        //Need to replace the cell's type ref with the parameter's, then continue

        //TODO: Memory management!
        parameter_ptr p = node.data;
        type_ref paramtype = p->type;
        cell_tree_set_type_ref(in, paramtype);
        //Refresh the information about our node
        node = cell_tree_data(in);
        node_type_ref = cell_tree_get_type_ref(in);
    }


    //If we're not in the applicative position, will need to emit an argpos equation
    if (!cell_tree_isapplicative(in)) {
        int pos = cell_tree_get_offset(in);
        //Get the type ref to the function we're dealing with
        type_ref func = get_cell_type_ref(cell_tree_child_data(cell_tree_parent(in), 0));
        //Generate the equation
        node_type_ref = type_ref_addargpos_eqn(node_type_ref, func, pos);
    }
    //Add the current node's type ref to the type_ref table and return
    table = type_ref_table_add(table, cell_tree_get_type_ref(in));
    return table;
}

int solve_apply_equation(type_ref node, is_result_of* eqn) {
    type_ref func = eqn->func;

    polymorph_ptr poly = type_ref_getpoly(func);
    //If the given function is a polymorph
    if (poly != NULL) {
        //Figure out what the return type must fall under
        TypeInfo constraint = polymorph_ptr_get_return_type(poly);
        
        //TODO: We don't __actually__ mean "restrict" here -- we need to check if the
        //returned type is a subtype of the node's current type. If not, then it's a type error

        return type_ref_restrict(node, constraint);
    }
    return 0; //TODO: Add other kinds of functions here!
}

int solve_argpos_equation(type_ref node, is_in_pos* eqn) {
    type_ref func = eqn->func; //The function the current node is supplied to 

    polymorph_ptr poly = type_ref_getpoly(func);
    //If the given function is a polymorph
    if (poly != NULL) {
        //Figure out what our parameter would need to fall under
        TypeInfo constraint = polymorph_ptr_get_parameter_type(poly, eqn->pos);

        //Restrict our node to fall under the new type
        //If we restrict it at all, we are active!
        return type_ref_restrict(node, constraint);
    }
    return 0; //TODO: Add other kinds of functions here!
}

//If we're given a polymorph, restrict it based upon the types of its arguments
int solve_polymorph_equation(type_ref node, is_polymorph* eqn) {
    polymorph_ptr oldpoly = eqn->poly;
    //Restrict the polymorph based on current knowledge
    eqn->poly = polymorph_ptr_restrict(oldpoly, eqn->args);
    if (oldpoly != eqn->poly) {
        //must've changed!
        return 1;
    }
    return 0; //Otherwise, must not have
}

//Solves the type equations associated with a given type_ref
int solve_type_ref_equations(type_ref in) {
    type_equation_dynarray eqns = type_ref_get_equations(in);

    int active = 0;
    int i;
    for (i=0; i < eqns.size; i++) {
        switch (eqns.begin[i].expr_kind) {
            case is_polymorph_kind:
                active = active || solve_polymorph_equation(in, &eqns.begin[i].expr.is_polymorph_entry);
                break;
            case is_in_pos_kind:
                active = active || solve_argpos_equation(in, &eqns.begin[i].expr.is_in_pos_entry);
                break;
            case is_result_of_kind:
                active = active || solve_apply_equation(in, &eqns.begin[i].expr.is_result_of_entry);
                break;
        }
    }
    return active;
}


//Method for solving type equations associated with a given set of type_refs
type_ref_table solve_type_equations(type_ref_table table) {
    //Basic method (for now): Iterate through every type_ref, and with each one, iterate through
    //every equation. For each equation of a given type, call a different function which returns an int
    // that represents the activity status of the rule. When there cease
    //to be active rules, we are done.
    //TODO: Switch to the smarter method of building a digraph of dependencies and propagating changes

    int active = 1;

    while (active) {
        active = 0; //Default to no proof of activity

        int i;
        for (i=0; i < table.size; i++) {
            //We are active if any of the type_refs has an active rule
            active = active || solve_type_ref_equations(table.begin[i]);
        }

        printf("\n"); //For now, debug statements!
        print_type_ref_table_equations(table, names);
    }
    return table;
}

//Infers types in a given function body
cell_tree infer_body(cell_tree in) {
    type_ref_table type_refs = gen_type_equations(in, type_ref_table_init()); //Generates type equations from the tree
    //Print them out (debugging)
    print_type_ref_table_equations(type_refs, names);
    type_refs = solve_type_equations(type_refs); //Solves type equations
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
    //TODO: Make it so we don't have global variable "names"!
    names = in.names;
    //in.funcs = polymorph_ptr_dynarray_map(in.funcs, &infer_polymorph);
    //TODO: FIXME: This is a really dumb way to debug for now
    int i;
    for (i=0; i < in.funcs.size; i++) {
        in.funcs.begin[i] = infer_polymorph(in.funcs.begin[i]);
    }
    return in;
}

