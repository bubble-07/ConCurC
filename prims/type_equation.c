#include "type_equation.h"

type_equation make_is_polymorph_equation(type_ref var, polymorph_ptr poly, type_ref_dynarray argtypes) {
    type_equation result;
    result.var = var;
    result.expr_kind = is_polymorph_kind;

    result.expr.is_polymorph_entry.poly = poly;
    result.expr.is_polymorph_entry.args = argtypes;
    return result;
}

type_equation make_is_subtype_equation(type_ref var, type_ref super) {
    type_equation result;
    result.var = var;
    result.expr_kind = is_subtype_kind;
    result.expr.is_subtype_entry.t = super;
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

is_polymorph get_poly_eqn(type_equation in) {
    return in.expr.is_polymorph_entry;
}
is_in_pos get_argpos_eqn(type_equation in) {
    return in.expr.is_in_pos_entry;
}
is_result_of get_apply_eqn(type_equation in) {
    return in.expr.is_result_of_entry;
}
is_subtype get_subtype_eqn(type_equation in) {
    return in.expr.is_subtype_entry;
}
is_equal get_equal_eqn(type_equation in) {
    return in.expr.is_equal_entry;
}

type_equation print_type_equation(type_equation in, nametable names) {
    print_type_ref(in.var, names);

    type_expr_kind kind = in.expr_kind;
    if (kind == is_polymorph_kind) {
        is_polymorph RH = get_poly_eqn(in);
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
        is_in_pos RH = get_argpos_eqn(in);
        printf("<: argpos( %d", RH.pos);
        printf(",");
        print_type_ref(RH.func, names);
        printf(")");

    }
    else if (kind == is_result_of_kind) {
        is_result_of RH = get_apply_eqn(in);
        printf("= apply(");
        print_type_ref(RH.func, names);
        printf(", [");
        print_type_ref_list(RH.args, names);
        printf("] )");
    }
    else if (kind == is_subtype_kind) {
        is_subtype RH = get_subtype_eqn(in);
        printf("<: ");
        print_type_ref(RH.t, names);
    }
    else if (kind == is_equal_kind) {
        is_equal RH = get_equal_eqn(in);
        printf("= ");
        print_type_ref(RH.t, names);
    }
    printf("\n");

    return in;
}
void print_type_equations(type_equation_dynarray in, nametable names) {
    int i;
    for (i=0; i < in.size; i++) {
        print_type_equation(in.begin[i], names);
    }
}

