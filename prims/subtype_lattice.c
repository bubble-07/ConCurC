#include "subtype_lattice.h"

subtype_lattice* lattice_add_subtype(subtype_lattice* in, typeslot subtype, polytype supertype) {
    in->subtypes = typeslot_dynarray_add(in->subtypes, subtype);
    in->head = polytype_dynarray_add(in->head, supertype);
    return in;
}

//Makes a polytype lattice with the supplied type_refs (TODO: backwards?)
subtype_lattice* make_lattice() {
    subtype_lattice* result = (subtype_lattice*) memalloc(sizeof(subtype_lattice));
    result->subtypes = typeslot_dynarray_make(1);
    result->head = polytype_dynarray_make(1);
    return result;
}

polytype_dynarray lattice_get_subtypes(polytype in, subtype_lattice* lattice) {

    polytype_dynarray result = polytype_dynarray_make(1);

    int i;
    for (i=0; i < lattice->head.size; i++) {
        //Try to match the head
        int success = polytype_pour(in, lattice->head.begin[i]);
        if (success) {
            //If it's a match, add the instantiated subtype to the result
            result = polytype_dynarray_add(result, typeslot_instantiate(lattice->subtypes.begin[i]));
        }
    }
    return result;
}
