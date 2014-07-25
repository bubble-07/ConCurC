#include "polytype.h"
#include "type_graph.h"
#include "subtype_lattice.h"
#include "type_ref_info.h"

//Prints a given polytype
void print_type(polytype in, nametable names) {
    print_TypeGraphRef(in.ref, names);
    if (!is_monotype(in)) {
        printf("(");
        print_typeslot_list(in.argtypes, names);
        printf(")");
    }
    return;
}

polytype make_polytype(TypeGraphRef in, typeslot_dynarray args) {
    polytype result;
    result.ref = in;
    result.argtypes = args;
    return result;
}

int polytype_numargs(polytype in) {
    return in.argtypes.size;
}

typeslot get_polytype_arg(polytype in, int pos) {
    return in.argtypes.begin[pos];
}
polytype make_monotype(noderef in) {
    return make_polytype(in, typeslot_dynarray_make(1));
}

polytype make_bottom_type() {
    return Bottom;
}
polytype make_unknown_type() {
    return Top;
}

int is_any(polytype in) {
    return in.ref == Top.ref;
}

int is_monotype(polytype in) {
    return (in.argtypes.size == 0);
}

typeslot_dynarray polytype_get_subtypes(polytype in) {
    type_graph_node node = get_graph_node(in.ref);
    return lattice_get_subtypes(in, node.lattice);
}

//Checks [trivially] if the two polytypes are equal
int polytype_trivial_eq(polytype one, polytype two) {
    if (one.ref != two.ref) {
        return 0; //Can't be equal, not the same constructor
    }
    return typeslot_dynarray_trivial_eq(one.argtypes, two.argtypes);
}

int type_eq(polytype one, polytype two) {
    //Equal iff each a subtype of the other
    return (is_subtype(one, two) && is_subtype(two, one));
}

polytype copy_type(polytype in) {
    return in; //FIXME: Actually copy!
}

//Given two types, returns their union ("Either")
polytype union_types(polytype a, polytype b) {
    if (polytype_trivial_eq(a, Bottom)) {
        return b;
    }
    if (polytype_trivial_eq(b, Bottom)) {
        return a;
    }
    if (polytype_trivial_eq(a, b)) {
        return a;
    }
    typeslot_dynarray args = typeslot_dynarray_make(1);
    args = typeslot_dynarray_add(args, typeslot_from_type(a));
    args = typeslot_dynarray_add(args, typeslot_from_type(b));
    return make_polytype(Either, args);
}

//Given a list of polytypes, returns one that represents the union of all
//Must robustly handle the case where we have bottom types
polytype union_typelist(polytype_dynarray in) {
    polytype result = Bottom;
    int i;
    for (i=0; i < in.size; i++) {
        result = union_types(result, in.begin[i]);
    }
    return result;
}

//Adds a type to a list of polytypes
polytype add_type(polytype_dynarray in, polytype a);

//Helper that restricts the given subtype list so it falls under a given type,
//and then returns a concatenated "either"-ified type
polytype restrict_type(polytype_dynarray in, polytype constraint) {
    polytype_dynarray result = polytype_dynarray_make(1);
    int i;
    for (i=0; i < in.size; i++) {
        result = polytype_dynarray_add(result, intersect_types(in.begin[i], constraint));
    }
    return union_typelist(result);
}

//Finds the set of all types that are subtypes of a and b.
//Note that this version assumes we have the transitive closure
polytype intersect_types(polytype a, polytype b) {
    polytype_dynarray result = polytype_dynarray_make(1);

    //Handle special cases where either a or b are "Any"
    if (is_any(a)) {
        return b;
    }
    if (is_any(b)) {
        return a;
    }

    //Is A possibly a subtype of B?
    if (Type_graph_possiblesubtype(UniverseGraph, a, b)) {
        //it is. Check if A __is__ B (trivially -- is it exactly the same?)
        if (polytype_trivial_eq(a, b)) {
            //Great, they're both the same. Add one to the result and return
            return a;
        }
        else {
            //Since A is possibly a subtype of B, but A is not B,
            //we should be able to find A somewhere under B. 
            //To do so, instantiate B's subtypes in the lattice and recurse
            //by restricting the set of B's subtypes by A.
            typeslot_dynarray b_subtypes = polytype_get_subtypes(b);
            //TODO: Eliminate the instantiation going on here
            return restrict_type(typeslot_dynarray_instantiate(b_subtypes), a);
        }
    }
    //A cannot subtype B
    //Check if A's children do so instead.
    //TODO: Remove instantiation going on here
    return restrict_type(typeslot_dynarray_instantiate(polytype_get_subtypes(a)), b);
}

//Returns "true" if the given types are disjoint
int types_are_disjoint(polytype a, polytype b) {
    //They're disjoint if their intersection is "Bottom"
    return polytype_trivial_eq(intersect_types(a, b), Bottom);
}

//Returns "true" if a is a subtype of b
int is_subtype(polytype a, polytype b) {
    if (polytype_trivial_eq(b, Top)) {
        return 1; //Everything is a subtype of "top"
    }
    //Is A possibly a subtype of b?
    if (Type_graph_possiblesubtype(UniverseGraph, a, b)) {
        //Is A the same as B?
        if (polytype_trivial_eq(a, b)) {
            return 1;
        }
        //Otherwise, must not be, so recurse on b's children
        //TODO: Eliminate instantiation here!
        polytype_dynarray b_subtypes = typeslot_dynarray_instantiate(polytype_get_subtypes(b));
        int i;
        for (i=0; i < b_subtypes.size; i++) {
            //If a is a subtype of any subtype of b
            if (is_subtype(a, b_subtypes.begin[i])) {
                //Then it's a subtype of b
                return 1;
            }
        }
    }
    //In all other cases, it can't be a subtype of b
    return 0;
}

int polytype_pour(polytype src, polytype dest) {
    if (polytype_numargs(src) != polytype_numargs(dest)) {
        return 0;
    }
    int i;
    for (i=0; i < polytype_numargs(dest); i++) {
        int success = typeslot_pour(get_polytype_arg(src, i), get_polytype_arg(dest, i));
        if (!success) {
            return 0;
        }
    }
    return 1; //Must've been successful
}
