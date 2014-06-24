#include "../libs/memoryman.h"
#include "type_ref.h"
#include "type_ref_info.h"

type_ref make_known_type_ref(TypeInfo in) {
    type_ref_info* info = memalloc(sizeof(type_ref_info));
    info->upperbound = in;
    info->equations = equation_set_init();
    type_ref_node* result = memalloc(sizeof(type_ref_node));
    result->data = info;
    result->kind = type_ref_representative; //This is the representative node
    return result;
}

type_ref make_unknown_type_ref() {
    return make_known_type_ref(make_unknown_type());
}

//Gets the representative node
type_ref find(type_ref in) {
    if (in->kind == type_ref_representative) {
        return in;
    }
    //Otherwise, follow the chain
    return find((type_ref) in->data);
}

int hash_type_ref(type_ref in) {
    return (int) find(in); //Return address of representative
}

int type_ref_eq(type_ref one, type_ref two) {
    return find(one) == find(two); //Equal iff representative addresses equal
}

//Gets the principal bounding type of the given typeref
TypeInfo type_ref_getbound(type_ref in) {
    type_ref rep = find(in);
    type_ref_info* info = rep->data;
    return info->upperbound;
}

//Adds the given type equation to the typeref
type_ref type_ref_add_equation(type_ref in, type_equation eqn) {
    type_ref rep = find(in);
    type_ref_info* info = rep->data;
    info->equations = equation_set_add(info->equations, eqn);
    return in;
}

type_equation_dynarray type_ref_get_equations(type_ref in) {
    type_ref rep = find(in);
    type_ref_info* info = rep->data;
    return info->equations;
}

polymorph_ptr type_ref_getpoly(type_ref in) {
    type_equation_dynarray eqns = type_ref_get_equations(in);
    int i;
    for (i=0; i < eqns.size; i++) {
        if (eqns.begin[i].expr_kind == is_polymorph_kind) {
            is_polymorph poly_eqn = get_poly_RH(eqns.begin[i]);
            return poly_eqn.poly;
        }
    }
    return NULL;
}


type_ref type_ref_addpoly_eqn(type_ref in, polymorph_ptr poly, type_ref_dynarray args) {
    return type_ref_add_equation(in, make_poly_eqn(poly, args));
}
type_ref type_ref_addargpos_eqn(type_ref in, type_ref func, int pos) {
    return type_ref_add_equation(in, make_argpos_eqn(pos, func));
}
type_ref type_ref_addapply_eqn(type_ref in, type_ref func, type_ref_dynarray args) {
    return type_ref_add_equation(in, make_apply_eqn(func, args));
}

int type_ref_restrict(type_ref in, TypeInfo bound) {
    type_ref rep = find(in);
    type_ref_info* info = rep->data;

    TypeInfo oldbound = copy_type(info->upperbound);

    info->upperbound = restrict_sum(info->upperbound, bound);
    return !type_eq(oldbound, info->upperbound); //Active if the upper bound has changed
}

//Really dumb way to print type refs FIXME: Make this generate sensible unique names
void print_type_ref(type_ref in, nametable names) {
    printf(" [");
    int tmp = (int) find(in);
    char printchar = (tmp % 27) + 64; //Make it some dumb printable letter/character
    printf("%c <: ", printchar);
    print_type(type_ref_getbound(in), names);
    printf("] ");
    return;
}

void print_type_ref_list(type_ref_dynarray in, nametable names) {
    int i;
    for (i=0; i < in.size; i++) {
        print_type_ref(in.begin[i], names);
        printf(",");
    }
    return;
}

