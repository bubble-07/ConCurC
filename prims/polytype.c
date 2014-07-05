#include "polytype.h"
#include "type_graph.h"
#include "subtype_lattice.h"

//Prints a given polytype
void print_polytype(polytype in, nametable names) {
    print_TypeGraphRef(in.ref, names);
    if (!is_monotype(in)) {
        printf("(");
        print_type_ref_list(in.argtypes, names);
        printf(")");
    }
    return;
}

polytype make_monotype(TypeGraphRef in) {
    polytype result;
    result.ref = in;
    result.argtypes = type_ref_dynarray_make(1);
    return result;
}

int is_any(polytype in) {
    return in.ref == Top.ref;
}

int is_monotype(polytype in) {
    return (in.argtypes.size == 0);
}

TypeInfo polytype_get_subtypes(polytype in) {
    type_graph_node node = get_graph_node(in.ref);
    return lattice_get_subtypes(in, node.lattice);
}

int polytype_trivial_eq(polytype one, polytype two) {
    if (one.ref != two.ref) {
        return 0; //Can't be equal, not the same constructor
    }
    return type_ref_dynarray_trivial_eq(one.argtypes, two.argtypes);
}
    

