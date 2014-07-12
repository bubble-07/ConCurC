#include "subtype_lattice.h"
#include "type_ref_info.h"

subtype_lattice* lattice_add_subtype(subtype_lattice* in, typeslot subtype, polytype supertype, type_ref_dynarray refs) {
    in->subtypes = typeslot_dynarray_add(in->subtypes, subtype);
    in->head = polytype_dynarray_add(in->head, supertype);
    in->refs = type_ref_dynarray_dynarray_add(in->refs, refs);
    return in;
}

//Makes a polytype lattice with the supplied type_refs (TODO: backwards?)
subtype_lattice* make_lattice() {
    subtype_lattice* result = (subtype_lattice*) memalloc(sizeof(subtype_lattice));
    result->subtypes = typeslot_dynarray_make(1);
    result->head = polytype_dynarray_make(1);
    result->refs = type_ref_dynarray_dynarray_make(1);
    return result;
}

typeslot_dynarray lattice_get_subtypes(polytype in, subtype_lattice* lattice) {

    typeslot_dynarray result = typeslot_dynarray_make(1);

    int i;
    for (i=0; i < lattice->head.size; i++) {
        //Record the bounds of all type_refs associated with the current head
        type_ref_info_ptr_dynarray bounds = get_type_ref_info_list(lattice->refs.begin[i]);

        //Try to match the head
        int success = polytype_pour(in, lattice->head.begin[i]);
        if (success) {
            //If it's a match, add the instantiated subtype to the result
            result = typeslot_dynarray_add(result, typeslot_dissociate(lattice->subtypes.begin[i]));
        }

        //Restore the bounds of all of the type_vars
        restore_type_ref_info_list(bounds, lattice->refs.begin[i]); 
    }
    return result;
}
