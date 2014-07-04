#include "typeinfo.h"

TypeInfo make_typeinfo(polytype_dynarray in) {
    TypeInfo result;
    result.options = in;
    return result;
}

//Prints the options in the given type
void print_type(TypeInfo in, nametable names) {
    int i;
    printf("Option[");
    for (i=0; i < in.options.size; i++) {
        print_polytype(in.options.begin[i], names);
        //If we're not dealing with the last element
        if (i < in.options.size - 1) {
            printf(", ");
        }
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

TypeInfo polytype_dynarray_to_TypeInfo(polytype_dynarray in) {
    TypeInfo result;
    result.options = in; //TODO: maybe make this copy?
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

//Finds the set of all types that are subtypes of a and b.
//Note that this version assumes we have the transitive closure
//TODO: Maybe make monotypes also have subtype lattices?

//TODO: Expand functionality!
TypeInfo intersect_types(polytype a, polytype b) {
    TypeInfo result = make_empty_type();

    //Is A possibly a subtype of B?
    if (Type_graph_testedge(UniverseGraph, b.ref, a.ref)) {
        //it is. Check if A __is__ B (trivially -- is it exactly the same?)
        if (polytype_trivial_eq(a, b)) {
            //Great, they're both the same. Add one to the result and return
            return add_type(result, a);
        }
        else {
            //Since A is possibly a subtype of B, but A is not B,
            //we should be able to find A somewhere under B. 
            //To do so, instantiate B's subtypes in the lattice and recurse
            //by restricting the set of B's subtypes by A.
            return restrict_type(polytype_get_subtypes(b), a);
        }
    }
    //A cannot subtype B
    //Check if A's children do so instead.
    return restrict_type(polytype_get_subtypes(a), b);
}

//Returns "true" if the given polytype is in the given typeinfo [equality checked trivially]
int trivial_is_in_TypeInfo(TypeInfo in, polytype t) {
    int i;
    for (i=0; i < in.options.size; i++) {
        if (polytype_trivial_eq(in.options.begin[i], t)) {
            return 1;
        }
    }
    return 0;
}

//Takes some TypeInfo, and eliminates options that are EXACTLY EQUAL [polytype_trivial_eq]
TypeInfo trivial_simplify_TypeInfo(TypeInfo in) {
    TypeInfo result = make_empty_type();
    //Copy incoming over into the result one-by-one, checking to see if there are duplicates
    int i;
    for (i=0; i < in.options.size; i++) {
        //If the current item is not already in the result
        if (!trivial_is_in_TypeInfo(result, in.options.begin[i])) {
            //Add it to the result
            result = add_type(result, in.options.begin[i]);
        }
    }
    return result;
}
        

//Takes some TypeInfo, eliminates redundant options,
//[types that turn out to be subtypes of other types in the typeinfo]
//and returns some new TypeInfo representing the simplified result.
//While inefficient as hell (O(n*n)), it's a necessary operation.
//NOTE: this operation destroys the TypeInfo passed in.

//TODO: Make this work with polymorphic type constructors, or restrict its operation
//to strictly deal with monotypes

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
    //return simplify_TypeInfo(result); 
    return trivial_simplify_TypeInfo(result); //TODO: fix simplification
}

//Concats the result of running restrict_type on "in" with the elements of "by"
//and then returns the simplified result
TypeInfo restrict_sum(TypeInfo in, TypeInfo by) {
    TypeInfo result = make_empty_type();
    int i;
    for (i=0; i < by.options.size; i++) {
        result = concat_types(result, restrict_type(in, by.options.begin[i]));
    }
    //return simplify_TypeInfo(result);
    return trivial_simplify_TypeInfo(result);
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
