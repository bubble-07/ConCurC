#include "subtype_lattice.h"

subtype_lattice* lattice_add_subtype(subtype_lattice* in, polytype subtype) {
    in->subtypes = polytype_dynarray_add(in->subtypes, subtype);
    return in;
}

subtype_lattice* make_monotype_lattice() {
    subtype_lattice* result = (subtype_lattice*) memalloc(sizeof(subtype_lattice));
    result->subtypes = polytype_dynarray_make(1);
    result->head = type_ref_dynarray_make(1);
    return result;
}

TypeInfo lattice_get_subtypes(polytype in, subtype_lattice* lattice) {
    /*
    //TODO: Cleanse references after you're done with them!

    //Firstly, we match the given polytype with the lattice's head
    //We do this by overwriting type_refs in the lattice's head with those of in
    //TODO: Handle matching!
    polytype_pour_args(in, lattice->head);
    //Then, by magical pointer-y action-at a distance, the subtypes will also have the correct type_refs
    //Return the list of subtypes.
    return poltype_dynarray_to_TypeInfo(lattice->subtypes);
    */

    //For now, just handle monotypes
    if (is_monotype(in)) {
        return make_typeinfo(lattice->subtypes);
    }

    return make_empty_type(); //For now, leave unimplemented to make sure everything still works for monotypes
}
