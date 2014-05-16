#ifndef TYPEDEFINED
typedef char Type; //For now, we'll have the Types actually contain no info

//Define a directed graph on types
DEFINE_GRAPH(Type)

typedef noderef TypeRef;

//Define a dictionary allowing us to look up nodes in the dictionary
//based upon the name of a type
DEFINE_DICT(string, TypeRef)

//Define a global type universe containing everything we want
Type_graph UniverseGraph;
//And define a global dictionary for looking up types
string_TypeRef_dict UniverseDict;
//Give a reference to the "Any" or "Top" type
TypeRef Top;

#endif
#ifdef TYPEDEFINED
extern Type_graph UniverseGraph; //Give access to the type universe
extern string_TypeRef_dict UniverseDict;
extern TypeRef Top;
#endif
#ifndef TYPEDEFINED
#define TYPEDEFINED

DEFINE_DYNARRAY(TypeRef)

typedef struct {
    TypeRef_dynarray options; //List of potential types
} TypeInfo;

//Macro for adding a type to the graph with the given name
#define AddType(name) noderef name; UniverseGraph = Type_graph_addnode(UniverseGraph, ' ', &name); \
                      UniverseDict = string_TypeRef_dict_add(UniverseDict, \
                                    string_TypeRef_bucket_make(to_dynstring(#name), &name));

//Macro for denoting that one type subtypes another in the graph
//Note that the arrow always points from supertype to subtype
#define Subs(one, two) UniverseGraph = Type_graph_addedge(UniverseGraph, two, one);


inline static void init_type_universe() {
    UniverseGraph = Type_graph_init(2);
    
    AddType(Any); //Add the ever-prevalent "Any" type

    Top = Any;

    //For now, we add the following. TODO: Move somewhere less hard-coded!
    AddType(String)
    AddType(Number)
    AddType(Int)
    AddType(Float)
    Subs(String, Any)
    Subs(Number, Any)
    Subs(Int, Number)
    Subs(Float, Number)

    //Great, let's fill out the rest automatically
    UniverseGraph = Type_graph_transitiveclosure(UniverseGraph);
    UniverseGraph = Type_graph_reflexiveclosure(UniverseGraph);
    return;
}

//Creates a new type that can't be anything.
inline static TypeInfo make_empty_type() {
    TypeInfo result;
    result.options = TypeRef_dynarray_make(1);
    return result;
}

//Adds a type to the given TypeInfo
inline static TypeInfo add_type(TypeInfo in, TypeRef a) {
    in.options = TypeRef_dynarray_add(in.options, a);
    return in;
}


//Creates a new type that could be anything.
inline static TypeInfo make_unknown_type() {
    return add_type(make_empty_type(), Top);
}

//Frees the given type
inline static void free_type(TypeInfo in) {
    TypeRef_dynarray_free(in.options);
    return;
}

//Note: we assume each TypeInfo reference is always unique.

//Finds the set of all types that are subtypes of a and b.
//Note that this version assumes we have the transitive closure
//TODO: Have this operate on the transitive reduction or a user-supplied graph,
//and make it based on a traversal instead! [For probably greater efficiency]

inline static TypeInfo intersect_types(TypeRef a, TypeRef b) {
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
    size_t i; //Will be used for indexing the source
    size_t j; //Will be used for the destination
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
            if (Type_graph_testedge(UniverseGraph, buf.options[j], in.options[i])) {
                included = 0; //don't include it
                break;
            }
        }
        if (included) {
            buf = add_type(buf, in.options[i]);
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
            if (Type_graph_testedge(UniverseGraph, in.options[j], buf.options[i])) {
                included = 0; //don't include it
                break;
            }
        }
        if (included) {
            in = add_type(in, buf.options[i]);
        }
    }

    //Finally, delete the information in the buf, and return in
    free_type(buf);
    return in;
}





    


#endif
