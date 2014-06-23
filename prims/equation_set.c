#include "equation_set.h"

eqn_set equation_set_init() {
    return type_equation_dynarray_make(1);
}

eqn_set equation_set_add(eqn_set s, type_equation eqn) {
    return type_equation_dynarray_add(s, eqn);
}

eqn_set equation_set_replace(eqn_set s, type_equation* ref, type_equation val) {
    *ref = val;
    return s;
}

//Gets the first type_equation of a given kind
type_equation* equation_set_getkind(eqn_set s, type_expr_kind k) {
    int i;
    for (i=0; i < s.size; i++) {
        if (s.begin[i].expr_kind == k) {
            return &s.begin[i];
        }
    }
    return NULL; //couldn't find it
}

//Gets all equations of a given kind
type_equation_ptr_dynarray equation_set_getall(eqn_set s, type_expr_kind k) {
    type_equation_ptr_dynarray result = type_equation_ptr_dynarray_make(1);
    int i;
    for (i=0; i < s.size; i++) {
        if (s.begin[i].expr_kind == k) {
            result = type_equation_ptr_dynarray_add(result, &s.begin[i]);
        }
    }
    return result;
}
        

void print_type_equations(eqn_set eqns, nametable names) {
    int i;
    for (i=0; i < eqns.size; i++) {
        printf("blah \n");
        print_type_equation(eqns.begin[i], names);
    }
    return;
}
    

