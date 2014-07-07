#include "typeslot.h"
#include "type_ref_info.h"

typeslot typeslot_from_ref(type_ref in) {
    typeslot result;
    result.data = in;
    result.kind = typeslot_ref;
    return result;
}
typeslot typeslot_from_type(polytype in) {
    typeslot result;
    polytype* ptr = memalloc(sizeof(polytype));
    *ptr = in;
    result.data = ptr;
    result.kind = typeslot_type;
    return result;
}
typeslot_kind typeslot_get_kind(typeslot in) {
    return in.kind;
}
//If the incoming typeslot is NOT a reference kind, return NULL
type_ref typeslot_get_ref(typeslot in) {
    if (in.kind != typeslot_ref) {
        return NULL;
    }
    return in.data;
}
//WARNING: UNSAFE if the incoming typeslot is not a polytype
polytype typeslot_get_type(typeslot in) {
    polytype* result = in.data;
    return *result;
}

void print_typeslot(typeslot in, nametable names) {
    if (in.kind == typeslot_ref) {
        print_type_ref(typeslot_get_ref(in), names);
    }
    print_type(typeslot_get_type(in), names);
}

void print_typeslot_list(typeslot_dynarray in, nametable names) {
    int i;
    for (i=0; i < in.size; i++) {
        print_typeslot(in.begin[i], names);
        printf(", ");
    }
}


int typeslot_trivial_eq(typeslot a, typeslot b) {
    if (a.kind != b.kind) {
        return 0;
    }
    if (a.kind == typeslot_ref) {
        //For now, don't do anything with the refs
        return 1;
    }
    return polytype_trivial_eq(typeslot_get_type(a), typeslot_get_type(b));
}
int typeslot_dynarray_trivial_eq(typeslot_dynarray a, typeslot_dynarray b) {
    if (a.size != b.size) { 
        return 0;
    }
    int i;
    for (i=0; i < a.size; i++) {
        if (!typeslot_trivial_eq(a.begin[i], b.begin[i])) {
            return 0;
        }
    }
    return 1;
}

int typeslot_pour(typeslot from, typeslot to) {
    //If the destination is a ref and the source is a polytype
    if (to.kind == typeslot_ref && from.kind == typeslot_type) {
        //Change the destination type_ref's bound
        type_ref destref = typeslot_get_ref(to);
        //Set the bounds correctly
        type_ref_setbound(destref, typeslot_get_type(from));
        return 1; //Success!
    }
    //If both are polytypes
    if (to.kind == typeslot_type && from.kind == typeslot_type) {
        polytype srctype = typeslot_get_type(from);
        polytype desttype = typeslot_get_type(to);

        //Their stems must be trivially equal
        if (srctype.ref == desttype.ref) {
            //Recurse!
            typeslot_dynarray_pour(srctype, desttype.argtypes);
        }
    }
    //TODO: Handle case that both are refs!
    return 0;
}

        

int typeslot_dynarray_pour(polytype from, typeslot_dynarray to) {
    if (polytype_numargs(from) != to.size) {
        return 0;
    }
    int i;
    for (i=0; i < to.size; i++) {
        int success = typeslot_pour(get_polytype_arg(from, i), to.begin[i]);
        if (!success) {
            return 0;
        }
    }
    return 1; //Must've been successful
}

polytype typeslot_instantiate(typeslot in) {
    //If we're dealing with a ref, just return its bound
    if (in.kind == typeslot_ref) {
        return type_ref_getbound(typeslot_get_ref(in));
    }
    //Otherwise, it must be a polytype
    polytype incoming = typeslot_get_type(in);
    
    //Create an instantiated version of the result's arguments
    polytype_dynarray resultargs = typeslot_dynarray_instantiate(incoming.argtypes);

    //Now, we need to package those BACK into typeslots
    typeslot_dynarray packedargs = typeslot_dynarray_make(1);
    int i;
    for (i=0; i < resultargs.size; i++) {
        packedargs = typeslot_dynarray_add(packedargs, typeslot_from_type(resultargs.begin[i]));
    }

    //Return our final instantiated copy
    return make_polytype(incoming.ref, packedargs);
}


polytype_dynarray typeslot_dynarray_instantiate(typeslot_dynarray in) {
    polytype_dynarray result = polytype_dynarray_make(1);
    int i;
    for (i=0; i < in.size; i++) {
        result = polytype_dynarray_add(result, typeslot_instantiate(in.begin[i]));
    }
    return result;
}
