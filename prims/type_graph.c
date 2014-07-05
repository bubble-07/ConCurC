#include "type_graph.h"
#include "type_ref.h"

void mono_subtypes_to_lattice(noderef super, noderef subtype) {
    type_graph_node supernode = Type_graph_getnode(UniverseGraph, super);
    supernode.lattice = lattice_add_subtype(supernode.lattice, make_monotype(subtype));
    return;
}

//Macro for adding a type to the graph with the given internal name (for reference to the node)
//and the given value for the LEXID (probably hardcoded into lexer.c and lexid.h)
#define AddMonoType(name, id) noderef name; UniverseGraph = Type_graph_addnode(UniverseGraph, make_type_graph_node(id, make_monotype_lattice()), &name); \
                      UniverseDict = lexid_TypeGraphRef_dict_add(UniverseDict, \
                                    lexid_TypeGraphRef_bucket_make(id, name));

//Adds a polytype with the given subtype lattice
#define AddPolyType(name, id, stl) noderef name; UniverseGraph = Type_graph_addnode(UniverseGraph, make_type_graph_node(id, stl), &name); \
                      UniverseDict = lexid_TypeGraphRef_dict_add(UniverseDict, \
                                    lexid_TypeGraphRef_bucket_make(id, name));

//Macro for denoting that one type subtypes another in the graph
//Note that the arrow always points from supertype to subtype
#define MonoSubs(one, two) UniverseGraph = Type_graph_addedge(UniverseGraph, two, one); mono_subtypes_to_lattice(two, one);


void init_type_universe() {
    UniverseGraph = Type_graph_init(2);
    UniverseDict = lexid_TypeGraphRef_dict_init(100);
    
    AddMonoType(Any, ANYID_LEXID); //Add the ever-prevalent "Any" type

    AddMonoType(Monot, MONOID_LEXID); //Add the "mono" type [used for bridging to monotypes from polytypes]

    Top = make_monotype(Any);
    Mono = Monot; //Share it globally

    //For now, we add the following. TODO: Move somewhere less hard-coded!
    AddMonoType(String, STRINGID_LEXID)
    AddMonoType(Num, NUMID_LEXID)
    AddMonoType(Int, INTID_LEXID)
    AddMonoType(Float, FLOATID_LEXID)


    MonoSubs(String, Any)
    MonoSubs(Num, Any)
    MonoSubs(Int, Num)
    MonoSubs(Float, Num)

    //Okay, now let's define Either in a hardcoded way
    type_ref a = make_unknown_type_ref();
    type_ref b = make_unknown_type_ref();
    type_ref_dynarray eitherargs = type_ref_dynarray_add(type_ref_dynarray_add(type_ref_dynarray_make(1), a), b);


    subtype_lattice* eitherlattice = make_polytype_lattice(eitherargs);
    polytype aType = make_polytype(Mono, type_ref_dynarray_add(type_ref_dynarray_make(1), a)); //Create parametric monotypes for a and b
    polytype bType = make_polytype(Mono, type_ref_dynarray_add(type_ref_dynarray_make(1), b));

    eitherlattice = lattice_add_subtype(eitherlattice, aType);
    eitherlattice = lattice_add_subtype(eitherlattice, bType);

    AddPolyType(Either, EITHERID_LEXID, eitherlattice)

    Type_graph_addedge(UniverseGraph, Either, Mono);
    Type_graph_addedge(UniverseGraph, Mono, Any);
    Type_graph_addedge(UniverseGraph, Any, Either); //Net result: anything can be a subtype of "Either"


    //Great, let's fill out the rest of the graph automatically
    UniverseGraph = Type_graph_transitiveclosure(UniverseGraph);
    UniverseGraph = Type_graph_reflexiveclosure(UniverseGraph);
    return;
}

//Returns "true" if it's possible a subtypes b using the type graph data
int Type_graph_possiblesubtype(Type_graph in, polytype a, polytype b) {
    return Type_graph_testedge(in, b.ref, a.ref);
}


TypeGraphRef get_TypeGraphRef(lexid s) {
    return lexid_TypeGraphRef_dict_get(UniverseDict, s);
}


//TODO: Write this better.
string get_type_name(TypeGraphRef r, nametable names) {
    lexid token = lexid_TypeGraphRef_dict_reverse_get(UniverseDict, r);
    return nametable_get(names, token);
}

type_graph_node get_graph_node(TypeGraphRef r) {
    return Type_graph_getnode(UniverseGraph, r);
}

void print_TypeGraphRef(TypeGraphRef r, nametable names) {
    string name = get_type_name(r, names);
    //If we were able to find it
    if (!string_eq(name, string_lookup_failure)) {
        printf("%s", to_cstring(name));
    }
    //TODO: if not, print some kind of error
    return;
}


