#include "typeinfo.h"

//Prints the options in the given type
void print_type(TypeInfo in, nametable names) {
    int i;
    printf("Option[ ");
    for (i=0; i < in.options.size; i++) {
        print_polytype(in.options.begin[i], names);
        printf(" ,");
    }
    printf("] ");
    return;
}

//Creates a new type that can't be anything.
TypeInfo make_empty_type() {
    TypeInfo result;
    result.options = polytype_dynarray_make(1);
    return result;
}

TypeInfo copy_type(TypeInfo in) {
    TypeInfo result;
    result.options = polytype_dynarray_copy(in.options);
    return result;
}

//Adds a type to the given TypeInfo
TypeInfo add_type(TypeInfo in, polytype a) {
    in.options = polytype_dynarray_add(in.options, a);
    return in;
}

//Makes a simple type from the given polytype
TypeInfo make_simple_type(polytype in) {
    TypeInfo result = add_type(make_empty_type(), in);
    return result;
}

TypeInfo make_simple_monotype(TypeGraphRef in) {
    return make_simple_type(make_monotype(in));
}

TypeInfo make_unknown_type() {
    return make_simple_type(Top);
}
//Determine if two types are equal
//TODO: Make this actually check properly
//ALSO TODO: Make this work on polytypes
int type_eq(TypeInfo one, TypeInfo two) {
    if (one.options.size != two.options.size) {
        return 0;
    }
    int i;
    for (i=0; i < one.options.size; i++) {
        if (one.options.begin[i].ref != two.options.begin[i].ref) {
            return 0;
        }
    }
    return 1;
}

//Frees the given type
void free_type(TypeInfo in) {
    polytype_dynarray_free(in.options);
    return;
}

//Concatenates [adds] the types of one to the types of two
//Then invalidates all references to two
TypeInfo concat_types(TypeInfo one, TypeInfo two) {
    one.options = polytype_dynarray_cat(one.options, two.options);
    return one;
}

//Note: we assume each TypeInfo reference is always unique.

//Finds the set of all types that are subtypes of a and b.
//Note that this version assumes we have the transitive closure
//TODO: Have this operate on the transitive reduction or a user-supplied graph,
//and make it based on a traversal instead! [For probably greater efficiency]

//TODO: Have this work on things other than monotypes!
TypeInfo intersect_types(polytype a, polytype b) {
    TypeInfo result = make_empty_type();
    size_t i;
    //Go through all subtypes of a, and add only those types
    //that are also subtypes of b.
    for (i=0; i < UniverseGraph.size; i++) {
        if (Type_graph_testedge(UniverseGraph, a.ref, i) && 
            Type_graph_testedge(UniverseGraph, b.ref, i)) {
            //Add the type at i to option list
            result = add_type(result, make_monotype(i));
        }
    }
    return result;
}


//Takes some TypeInfo, eliminates redundant options,
//[types that turn out to be subtypes of other types in the typeinfo]
//and returns some new TypeInfo representing the simplified result.
//While inefficient as hell (O(n*n)), it's a necessary operation.
//NOTE: this operation destroys the TypeInfo passed in.

TypeInfo simplify_TypeInfo(TypeInfo in) {
    int i; //Will be used for indexing the source
    int j; //Will be used for the destination
    int included; //Flag indicating that the current elem will be added

    //Pass 1: Copy into a buffer from left to right, and enforce 
    //that every element can't subtype any element to its left
    TypeInfo buf = make_empty_type();
    for (i=0; i < in.options.size; i++) {
        //Check against each element of the buf
        //If it isn't a subtype of anything in the buf, add it
        included = 1;
        for (j=0; j < buf.options.size; j++) {
            //If elem at i is a subtype of anything in the buf
            if (Type_graph_testedge(UniverseGraph, buf.options.begin[j].ref, in.options.begin[i].ref)) {
                included = 0; //don't include it
                break;
            }
        }
        if (included) {
            buf = add_type(buf, in.options.begin[i]);
        }
    }

    //Delete "in", and make it into a new TypeInfo
    free_type(in);
    in = make_empty_type();

    //Pass 2: Copy back into "in" from right to left, which
    //will reverse the options and enforce that any element can't
    //subtype any element to its left, resulting in a simplified list
    for (i=buf.options.size - 1; i >= 0; i--) {
        //Check against each element of "in"
        //If it isn't a subtype of anything in "in", add it
        included = 1;
        for (j=0; j < in.options.size; j++) {
            //If elem at i is a subtype of anything in "in"
            if (Type_graph_testedge(UniverseGraph, in.options.begin[j].ref, buf.options.begin[i].ref)) {
                printf("Won't be included \n");
                included = 0; //don't include it
                break;
            }
        }
        if (included) {
            in = add_type(in, buf.options.begin[i]);
        }
    }

    //Finally, delete the information in the buf, and return in
    free_type(buf);
    return in;
}

//Intersects a with all elements of in, sums them, and simplifies
TypeInfo restrict_type(TypeInfo in, polytype a) {
    TypeInfo result = make_empty_type();
    int i;
    for (i=0; i < in.options.size; i++) {
        result = concat_types(result, intersect_types(in.options.begin[i], a));
    }
    return simplify_TypeInfo(result);
}

//Concats the result of running restrict_type on "in" with the elements of "by"
//and then returns the simplified result
TypeInfo restrict_sum(TypeInfo in, TypeInfo by) {
    TypeInfo result = make_empty_type();
    int i;
    for (i=0; i < by.options.size; i++) {
        result = concat_types(result, restrict_type(in, by.options.begin[i]));
    }
    return simplify_TypeInfo(result);
}

//Returns "true" if one can't possibly be anything
int type_is_empty(TypeInfo one) {
    return one.options.size == 0;
}
 
//Returns true if the given types have nothing in common (neither subtypes the other)
int types_are_disjoint(TypeInfo one, TypeInfo two) {
    //The above condition is true iff the result of restricting the sum is nil
    return type_is_empty(restrict_sum(one, two));
}
