#include "../libs/dynstring.h"
#include "../libs/dict.h"
#include "../libs/digraph.h"
#include "../prims/lexid.h"
#include "../prims/nametable.h"

#ifdef TYPEDEFINED
extern Type_graph UniverseGraph; //Give access to the type universe
extern lexid_TypeGraphRef_dict UniverseDict;
extern TypeGraphRef Top;
#endif

#ifndef TYPEDEFINED
#define TYPEDEFINED

typedef char Type; //For now, we'll have the Types actually contain no info

//Define a directed graph on types
DEFINE_GRAPH(Type)

typedef noderef TypeGraphRef;

static noderef TypeGraphRef_lookup_failure = -1; //define -1 to be failure

static int TypeGraphRef_eq(TypeGraphRef a, TypeGraphRef b) {
    return a == b;
}

//Define a dictionary allowing us to look up nodes in the dictionary
//based upon the name of a type
DEFINE_DICT(lexid, TypeGraphRef)



DEFINE_REVERSE_LOOKUP(lexid, TypeGraphRef)

//Define a global type universe containing everything we want
Type_graph UniverseGraph;
//And define a global dictionary for looking up types
lexid_TypeGraphRef_dict UniverseDict;
//Give a reference to the "Any" or "Top" type
TypeGraphRef Top;

DEFINE_DYNARRAY(TypeGraphRef)

typedef struct {
    TypeGraphRef_dynarray options; //List of potential types
} TypeInfo;

//Macro for adding a type to the graph with the given internal name (for reference to the node)
//and the given value for the LEXID (probably hardcoded into lexer.c and lexid.h)
#define AddType(name, id) noderef name; UniverseGraph = Type_graph_addnode(UniverseGraph, ' ', &name); \
                      UniverseDict = lexid_TypeGraphRef_dict_add(UniverseDict, \
                                    lexid_TypeGraphRef_bucket_make(id, name));

//Macro for denoting that one type subtypes another in the graph
//Note that the arrow always points from supertype to subtype
#define Subs(one, two) UniverseGraph = Type_graph_addedge(UniverseGraph, two, one);


inline static void init_type_universe() {
    UniverseGraph = Type_graph_init(2);
    UniverseDict = lexid_TypeGraphRef_dict_init(100);
    
    AddType(Any, ANYID_LEXID); //Add the ever-prevalent "Any" type

    Top = Any;

    //For now, we add the following. TODO: Move somewhere less hard-coded!
    AddType(String, STRINGID_LEXID)
    AddType(Num, NUMID_LEXID)
    AddType(Int, INTID_LEXID)
    AddType(Float, FLOATID_LEXID)


    Subs(String, Any)
    Subs(Num, Any)
    Subs(Int, Num)
    Subs(Float, Num)

    //Great, let's fill out the rest automatically
    UniverseGraph = Type_graph_transitiveclosure(UniverseGraph);
    UniverseGraph = Type_graph_reflexiveclosure(UniverseGraph);
    return;
}

static TypeGraphRef get_TypeGraphRef(lexid s) {
    return lexid_TypeGraphRef_dict_get(UniverseDict, s);
}


//TODO: Write this better.
static string get_type_name(TypeGraphRef r, nametable names) {
    lexid token = lexid_TypeGraphRef_dict_reverse_get(UniverseDict, r);
    return nametable_get(names, token);
}

static void print_TypeGraphRef(TypeGraphRef r, nametable names) {
    string name = get_type_name(r, names);
    //If we were able to find it
    if (!string_eq(name, string_lookup_failure)) {
        printf("%s", to_cstring(name));
    }
    //TODO: if not, print some kind of error
    return;
}

static void print_type(TypeInfo in, nametable names) {
    int i;
    printf("Option[ ");
    for (i=0; i < in.options.size; i++) {
        print_TypeGraphRef(in.options.begin[i], names);
        printf(" ,");
    }
    printf("] ");
    return;
}

        

//Creates a new type that can't be anything.
inline static TypeInfo make_empty_type() {
    TypeInfo result;
    result.options = TypeGraphRef_dynarray_make(1);
    return result;
}

inline static TypeInfo copy_type(TypeInfo in) {
    TypeInfo result;
    result.options = TypeGraphRef_dynarray_copy(in.options);
    return result;
}

//Adds a type to the given TypeInfo
inline static TypeInfo add_type(TypeInfo in, TypeGraphRef a) {
    in.options = TypeGraphRef_dynarray_add(in.options, a);
    return in;
}

//Makes a simple type from the given type graph reference
inline static TypeInfo make_simple_type(TypeGraphRef in) {
    TypeInfo result = add_type(make_empty_type(), in);
    return result;
}

inline static TypeInfo make_unknown_type() {
    return make_simple_type(Top);
}

//Determine if two types are equal
//TODO: Make this actually check properly
inline static int type_eq(TypeInfo one, TypeInfo two) {
    if (one.options.size != two.options.size) {
        return 0;
    }
    int i;
    for (i=0; i < one.options.size; i++) {
        if (one.options.begin[i] != two.options.begin[i]) {
            return 0;
        }
    }
    return 1;
}
    

//Frees the given type
inline static void free_type(TypeInfo in) {
    TypeGraphRef_dynarray_free(in.options);
    return;
}

//Concatenates [adds] the types of one to the types of two
//Then invalidates all references to two
inline static TypeInfo concat_types(TypeInfo one, TypeInfo two) {
    one.options = TypeGraphRef_dynarray_cat(one.options, two.options);
    return one;
}

//Note: we assume each TypeInfo reference is always unique.

//Finds the set of all types that are subtypes of a and b.
//Note that this version assumes we have the transitive closure
//TODO: Have this operate on the transitive reduction or a user-supplied graph,
//and make it based on a traversal instead! [For probably greater efficiency]

inline static TypeInfo intersect_types(TypeGraphRef a, TypeGraphRef b) {
    TypeInfo result = make_empty_type();
    size_t i;
    //Go through all subtypes of a, and add only those types
    //that are also subtypes of b.
    for (i=0; i < UniverseGraph.size; i++) {
        if (Type_graph_testedge(UniverseGraph, a, i) && 
            Type_graph_testedge(UniverseGraph, b, i)) {
            //Add the type at i to option list
            result = add_type(result, i);
        }
    }
    return result;
}


//Takes some TypeInfo, eliminates redundant options,
//[types that turn out to be subtypes of other types in the typeinfo]
//and returns some new TypeInfo representing the simplified result.
//While inefficient as hell (O(n*n)), it's a necessary operation.
//NOTE: this operation destroys the TypeInfo passed in.

inline static TypeInfo simplify_TypeInfo(TypeInfo in) {
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
            if (Type_graph_testedge(UniverseGraph, buf.options.begin[j], in.options.begin[i])) {
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
            if (Type_graph_testedge(UniverseGraph, in.options.begin[j], buf.options.begin[i])) {
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
static TypeInfo restrict_type(TypeInfo in, TypeGraphRef a) {
    TypeInfo result = make_empty_type();
    int i;
    for (i=0; i < in.options.size; i++) {
        result = concat_types(result, intersect_types(in.options.begin[i], a));
    }
    return simplify_TypeInfo(result);
}

//Concats the result of running restrict_type on "in" with the elements of "by"
//and then returns the simplified result
static TypeInfo restrict_sum(TypeInfo in, TypeInfo by) {
    TypeInfo result = make_empty_type();
    int i;
    for (i=0; i < by.options.size; i++) {
        result = concat_types(result, restrict_type(in, by.options.begin[i]));
    }
    return simplify_TypeInfo(result);
}

//Returns "true" if one can't possibly be anything
inline static int type_is_empty(TypeInfo one) {
    return one.options.size == 0;
}
 
//Returns true if the given types have nothing in common (neither subtypes the other)
inline static int types_are_disjoint(TypeInfo one, TypeInfo two) {
    //The above condition is true iff the result of restricting the sum is nil
    return type_is_empty(restrict_sum(one, two));
}
   
#endif
