#include "type_graph.h"

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

//Macro for denoting that one type subtypes another in the graph
//Note that the arrow always points from supertype to subtype
#define MonoSubs(one, two) UniverseGraph = Type_graph_addedge(UniverseGraph, two, one); mono_subtypes_to_lattice(two, one);


void init_type_universe() {
    UniverseGraph = Type_graph_init(2);
    UniverseDict = lexid_TypeGraphRef_dict_init(100);
    
    AddMonoType(Any, ANYID_LEXID); //Add the ever-prevalent "Any" type

    Top = make_monotype(Any);

    //For now, we add the following. TODO: Move somewhere less hard-coded!
    AddMonoType(String, STRINGID_LEXID)
    AddMonoType(Num, NUMID_LEXID)
    AddMonoType(Int, INTID_LEXID)
    AddMonoType(Float, FLOATID_LEXID)


    MonoSubs(String, Any)
    MonoSubs(Num, Any)
    MonoSubs(Int, Num)
    MonoSubs(Float, Num)

    //Great, let's fill out the rest automatically
    UniverseGraph = Type_graph_transitiveclosure(UniverseGraph);
    UniverseGraph = Type_graph_reflexiveclosure(UniverseGraph);
    return;
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


