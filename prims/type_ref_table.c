#include "type_ref_table.h"

type_ref_table type_ref_table_add(type_ref_table in, type_ref t) {
    in = type_ref_dynarray_add(in, t);
    return in;
}
type_ref_table type_ref_table_init() {
    return type_ref_dynarray_make(1);
}

void print_type_ref_table_equations(type_ref_table in, nametable names) {
    int i;
    for (i=0; i < in.size; i++) {
        print_type_ref(in.begin[i], names);
        type_equation_dynarray eqns = type_ref_get_equations(in.begin[i]);
        int j;
        for (j=0; j < eqns.size; j++) {
            print_type_equation(eqns.begin[j], names);
        }
    }
    return;
}
        

