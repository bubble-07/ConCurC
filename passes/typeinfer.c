#include "typeinfer.h"
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
equation_set gen_type_equations(cell_tree in, equation_set eqns) {
    //Do everything depth-first and RIGHT TO LEFT (otherwise, polymorph eqns won't emit correctly)
    int i;
    for (i=cell_tree_numchildren(in) - 1; i >= 0; i--) {
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
        //Generate and add "polymorph" equation
        eqns = equation_set_addpoly(eqns, node_type_ref, poly, argtypes);
    }

    //If we're dealing with a compound expression
    if (kind == EXPRCELL) {
        type_ref_dynarray argtypes = get_arg_type_refs(in);
        type_ref functype = get_cell_type_ref(cell_tree_child_data(in, 0));
        //Generate and add "apply" equation
        eqns = equation_set_addapply(eqns, node_type_ref, functype, argtypes);
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
        eqns = equation_set_addargpos(eqns, node_type_ref, func, pos);
    }

    return eqns;
}

rule_app_result expand_argpos_rule(type_equation* argpos_eqn, type_equation* poly_eqn, equation_set eqns) {
    rule_app_result result = rule_app_result_init(eqns);
    result.active = 1; //We are active!

    is_in_pos argpos_RH = get_argpos_eqn(*argpos_eqn);
    is_polymorph poly_RH = get_poly_eqn(*poly_eqn); 
    //Get a reference to the polymorph we're dealing with
    polymorph_ptr poly = poly_RH.poly;
    //Figure out what our parameter would need to fall under
    type_ref constraint = polymorph_ptr_get_parameter_type(poly, argpos_RH.pos);

    //Generate the new subtype constraint equation
    type_equation sub_eqn = make_subtype_eqn(argpos_eqn->var, constraint);

    //If there is one, and only one option for the polymorph
    if (polymorph_ptr_numoptions(poly) == 1) {
        //Replace our incoming equation in-place
        *argpos_eqn = sub_eqn;
        return result;
    }
    else {
        //Add it to the end
        eqns = equation_set_add(eqns, sub_eqn);
    }

    result.eqns = eqns;
    return result;
}


rule_app_result expand_argpos_foreach(type_equation* argpos_eqn, equation_set eqns) {

    is_in_pos argpos_RH = get_argpos_eqn(*argpos_eqn);

    return with_polymorph(argpos_eqn, argpos_RH.func, eqns, &expand_argpos_rule);
}

//This rule expands any "is_in_pos" equations to generate new constraints (if possible)
//If the first argument of the "is_in_pos" is a type equation that points to a polymorph
//that can only take on one type, the "is_in_pos" equation will replaced with the new constraint
rule_app_result expand_argpos(equation_set eqns) {
    return for_every(is_in_pos_kind, eqns, &expand_argpos_foreach);
}

//TODO: FIXME: Find this based upon BEST CURRENT KNOWLEDGE of parameter types! [trickier than the dumb way!]
rule_app_result expand_apply_rule(type_equation* apply_eqn, type_equation* poly_eqn, equation_set eqns) {
    is_result_of apply_RH = get_apply_eqn(*apply_eqn);
    is_polymorph poly_RH = get_poly_eqn(*poly_eqn);
} 

rule_app_result expand_apply_foreach(type_equation* apply_eqn, equation_set eqns) {
    is_result_of apply_RH = get_apply_eqn(*apply_eqn);
    return with_polymorph(apply_eqn, apply_RH.func, eqns, &expand_apply_rule);
}


rule_app_result expand_apply(equation_set eqns) {
    return for_every(is_result_of_kind, eqns, &expand_apply_foreach);
}

//To be used with "solve_type_equations"
#define APPLY_RULE(rule) \
    tmp_result = rule(eqns); \
    active = active || tmp_result.active; /* Thread activity through, if there is any */ \
    eqns = tmp_result.eqns; //Mutate our equations


//Method for solving type equations
equation_set solve_type_equations(equation_set eqns) {
    //Basic method (for now): Apply rules until no more rules apply
    //TODO: Look at rule application process, figure out how to recursively check instead

    int active = 1; //This will be "1" if any rules were applied
    rule_app_result tmp_result; //Will store temporary results of applying rules

    while (active) {
        active = 0; //Default to no proof of activity

        APPLY_RULE(expand_argpos)
        //APPLY_RULE(restrict_poly)
        //APPLY_RULE(expand_apply)

        printf("\n"); //For now, debug statements!
        print_type_equations(eqns, names);
    }
}

//Infers types in a given function body
cell_tree infer_body(cell_tree in) {
    equation_set equations = gen_type_equations(in, equation_set_init()); //Generates type equations from the tree
    //Print them out (debugging)
    print_type_equations(equations, names);
    equations = solve_type_equations(equations); //Solves type equations
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
    for (i=2; i < in.funcs.size; i++) {
        in.funcs.begin[i] = infer_polymorph(in.funcs.begin[i]);
    }
    return in;
}

