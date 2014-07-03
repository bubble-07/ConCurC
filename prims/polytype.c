#include "polytype.h"
#include "type_graph.h"
#include "subtype_lattice.h"

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

int is_monotype(polytype in) {
    return (in.argtypes.size == 0);
}

TypeInfo polytype_get_subtypes(polytype in) {

    //Handle the special case that we have a monotype (no lattice)
    //TODO: Handle cycles!
    if (is_monotype(in)) {
        size_t i;
        TypeInfo result = make_empty_type();
        for (i=0; i < UniverseGraph.size; i++) {
            //Add all subtypes of the incoming node
            if (!noderef_eq(in.ref, i) && Type_graph_testedge(UniverseGraph, in.ref, i)) {
                //FIXME: This gets the representation wrong if a polytype subtypes a monotype!
                result = add_type(result, make_monotype(i));
            }
        }
        return result;
    }

    type_graph_node node = get_graph_node(in.ref);
    if (node.lattice == NULL) {
        return make_empty_type(); //For now, just do nothing
    }
    return lattice_get_subtypes(in, node.lattice);
}

int polytype_trivial_eq(polytype one, polytype two) {
    if (one.ref != two.ref) {
        return 0; //Can't be equal, not the same constructor
    }
    return type_ref_dynarray_trivial_eq(one.argtypes, two.argtypes);
}
    

