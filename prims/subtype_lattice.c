#include "subtype_lattice.h"

subtype_lattice* lattice_add_subtype(subtype_lattice* in, typeslot subtype) {
    in->subtypes = typeslot_dynarray_add(in->subtypes, subtype);
    return in;
}

//Makes a polytype lattice with the supplied type_refs (TODO: backwards?)
subtype_lattice* make_polytype_lattice(typeslot_dynarray header) {
    subtype_lattice* result = (subtype_lattice*) memalloc(sizeof(subtype_lattice));
    result->subtypes = typeslot_dynarray_make(1);
    result->head = header;
    return result;
}
subtype_lattice* make_monotype_lattice() {
    return make_polytype_lattice(typeslot_dynarray_make(1));
}

polytype_dynarray lattice_get_subtypes(polytype in, subtype_lattice* lattice) {

    //Pour the given polytype's argument types into the refs within the head
    typeslot_dynarray_pour(in, lattice->head);

    //Flatten all refs
    //TODO: Make it so it flattens ONLY the refs that are used here in a way
    //that avoids type_ref capture
    polytype_dynarray result = typeslot_dynarray_instantiate(lattice->subtypes);

    return result;
}
