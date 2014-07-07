#include "type_graph.h"
#include "type_ref.h"

void mono_subtypes_to_lattice(noderef super, noderef subtype) {
    type_graph_node supernode = Type_graph_getnode(UniverseGraph, super);
    supernode.lattice = lattice_add_subtype(supernode.lattice, typeslot_from_type(make_monotype(subtype)));
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

    Top = make_monotype(Any);

    AddMonoType(None, NONEID_LEXID); //Add the error state "null" type
    Bottom = make_monotype(None);
       
    //Okay, now let's define Either in a hardcoded way
    typeslot a = typeslot_from_ref(make_unknown_type_ref());
    typeslot b = typeslot_from_ref(make_unknown_type_ref());
    typeslot_dynarray eitherargs = typeslot_dynarray_add(typeslot_dynarray_add(typeslot_dynarray_make(1), a), b);


    subtype_lattice* eitherlattice = make_polytype_lattice(eitherargs);

    eitherlattice = lattice_add_subtype(eitherlattice, a);
    eitherlattice = lattice_add_subtype(eitherlattice, b);
 
    AddPolyType(Eithert, EITHERID_LEXID, eitherlattice)
    Either = Eithert;
    UniverseGraph = Type_graph_addedge(UniverseGraph, Any, Either);
    UniverseGraph = Type_graph_addedge(UniverseGraph, Either, Any); //Any type can subtype Either

    //For now, we add the following. TODO: Move somewhere less hard-coded!
    AddMonoType(String, STRINGID_LEXID)
    AddMonoType(Num, NUMID_LEXID)
    AddMonoType(Int, INTID_LEXID)
    AddMonoType(Float, FLOATID_LEXID)


    MonoSubs(String, Any)
    MonoSubs(Num, Any)
    MonoSubs(Int, Num)
    MonoSubs(Float, Num)

    //Great, let's fill out the rest of the graph automatically
    UniverseGraph = Type_graph_transitiveclosure(UniverseGraph);
    UniverseGraph = Type_graph_reflexiveclosure(UniverseGraph);
    return;
}

//Returns "true" if it's possible a subtypes b using the type graph data
int Type_graph_possiblesubtype(Type_graph in, polytype a, polytype b) {
    //Just doing this for now
    if (b.ref == Either) {
        return 1;
    }
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


