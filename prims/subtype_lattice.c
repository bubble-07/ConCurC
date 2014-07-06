#include "subtype_lattice.h"

subtype_lattice* lattice_add_subtype(subtype_lattice* in, polytype subtype) {
    in->subtypes = polytype_dynarray_add(in->subtypes, subtype);
    return in;
}

//Makes a polytype lattice with the supplied type_refs (TODO: backwards?)
subtype_lattice* make_polytype_lattice(type_ref_dynarray header) {
    subtype_lattice* result = (subtype_lattice*) memalloc(sizeof(subtype_lattice));
    result->subtypes = polytype_dynarray_make(1);
    result->head = header;
    return result;
}
subtype_lattice* make_monotype_lattice() {
    return make_polytype_lattice(type_ref_dynarray_make(1));
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

    if (is_monotype(in)) {
        return make_typeinfo(lattice->subtypes);
    }
    //Merge the information from the incoming polytype into the head [overwrite the head's typerefs]
    //polytype_pour_args(in, lattice->head);
    //TODO: SANITIZE CHILD REFERENCES
    return make_typeinfo(lattice->subtypes);

}
