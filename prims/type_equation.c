#include "type_equation.h"

type_equation make_is_polymorph_equation(type_ref var, polymorph_ptr poly, type_ref_dynarray argtypes) {
    type_equation result;
    result.var = var;
    result.expr_kind = is_polymorph_kind;

    result.expr.is_polymorph_entry.poly = poly;
    result.expr.is_polymorph_entry.args = argtypes;
    return result;
}

type_equation make_is_in_pos_equation(type_ref var, int pos, type_ref functype) {
    type_equation result;
    result.var = var;
    result.expr_kind = is_in_pos_kind;

    result.expr.is_in_pos_entry.pos = pos;
    result.expr.is_in_pos_entry.func = functype;
    return result;
}

type_equation make_is_result_of_equation(type_ref var, type_ref functype, type_ref_dynarray args) {
    type_equation result;
    result.var = var;
    result.expr_kind = is_result_of_kind;

    result.expr.is_result_of_entry.func = functype;
    result.expr.is_result_of_entry.args = args;
    return result;
}


