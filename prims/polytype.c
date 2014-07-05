#include "polytype.h"
#include "type_graph.h"
#include "subtype_lattice.h"
#include "type_ref_info.h"

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

polytype make_polytype(TypeGraphRef in, type_ref_dynarray args) {
    polytype result;
    result.ref = in;
    result.argtypes = args;
    return result;
}
polytype make_monotype(noderef in) {
    return make_polytype(in, type_ref_dynarray_make(1));
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

//Checks [trivially] if the two polytypes are equal
int polytype_trivial_eq(polytype one, polytype two) {
    if (one.ref != two.ref) {
        //Deal with the special case of "Mono" as a constructor

        if (is_monotype(one) && two.ref == Mono) {
            //Deconstruct "Mono" into the type it references
            TypeInfo twobound = type_ref_getbound(two.argtypes.begin[0]);
            return polytype_trivial_eq(one, twobound.options.begin[0]);
        }
        else if (is_monotype(two) && one.ref == Mono) {
            return polytype_trivial_eq(two, one); //Reverse and try again
        }
        return 0; //Can't be equal, not the same constructor
    }
    return type_ref_dynarray_trivial_eq(one.argtypes, two.argtypes);
}
    

