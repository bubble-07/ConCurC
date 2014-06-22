#include "type_equation.h"

type_equation make_poly_eqn(polymorph_ptr poly, type_ref_dynarray argtypes) {
    type_equation result;
    result.expr_kind = is_polymorph_kind;

    result.expr.is_polymorph_entry.poly = poly;
    result.expr.is_polymorph_entry.args = argtypes;
    return result;
}

type_equation make_argpos_eqn(int pos, type_ref functype) {
    type_equation result;
    result.expr_kind = is_in_pos_kind;

    result.expr.is_in_pos_entry.pos = pos;
    result.expr.is_in_pos_entry.func = functype;
    return result;
}

type_equation make_apply_eqn(type_ref functype, type_ref_dynarray args) {
    type_equation result;
    result.expr_kind = is_result_of_kind;

    result.expr.is_result_of_entry.func = functype;
    result.expr.is_result_of_entry.args = args;
    return result;
}

is_polymorph get_poly_RH(type_equation in) {
    return in.expr.is_polymorph_entry;
}
is_in_pos get_argpos_RH(type_equation in) {
    return in.expr.is_in_pos_entry;
}
is_result_of get_apply_RH(type_equation in) {
    return in.expr.is_result_of_entry;
}

type_equation print_type_equation(type_equation in, nametable names) {

    type_expr_kind kind = in.expr_kind;
    if (kind == is_polymorph_kind) {
        is_polymorph RH = get_poly_RH(in);
        printf("= poly(");
        //TODO: Print available polymorph types
        //For now, just print the name
        //print_polymorph_ptr_name(RH.poly);
        printf(", [");

        //Print argument types
        print_type_ref_list(RH.args, names);

        printf("] )");
    }
    else if (kind == is_in_pos_kind) {
        is_in_pos RH = get_argpos_RH(in);
        printf("<: argpos( %d", RH.pos);
        printf(",");
        print_type_ref(RH.func, names);
        printf(")");

    }
    else if (kind == is_result_of_kind) {
        is_result_of RH = get_apply_RH(in);
        printf("= apply(");
        print_type_ref(RH.func, names);
        printf(", [");
        print_type_ref_list(RH.args, names);
        printf("] )");
    }
    printf("\n");

    return in;
}


