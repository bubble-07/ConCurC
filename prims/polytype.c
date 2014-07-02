#include "polytype.h"
#include "type_graph.h"

//Prints a given polytype
void print_polytype(polytype in, nametable names) {
    print_TypeGraphRef(in.ref, names);
    printf("(");
    print_type_ref_list(in.argtypes, names);
    printf(")");
    return;
}

polytype make_monotype(TypeGraphRef in) {
    polytype result;
    result.ref = in;
    result.argtypes = type_ref_dynarray_make(1);
    return result;
}
    
