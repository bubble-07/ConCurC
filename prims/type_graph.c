#include "type_graph.h"

//Macro for adding a type to the graph with the given internal name (for reference to the node)
//and the given value for the LEXID (probably hardcoded into lexer.c and lexid.h)
#define AddType(name, id) noderef name; UniverseGraph = Type_graph_addnode(UniverseGraph, make_type_graph_node(id, NULL), &name); \
                      UniverseDict = lexid_TypeGraphRef_dict_add(UniverseDict, \
                                    lexid_TypeGraphRef_bucket_make(id, name));

//Macro for denoting that one type subtypes another in the graph
//Note that the arrow always points from supertype to subtype
#define Subs(one, two) UniverseGraph = Type_graph_addedge(UniverseGraph, two, one);


void init_type_universe() {
    UniverseGraph = Type_graph_init(2);
    UniverseDict = lexid_TypeGraphRef_dict_init(100);
    
    AddType(Any, ANYID_LEXID); //Add the ever-prevalent "Any" type

    Top = make_monotype(Any);

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

TypeGraphRef get_TypeGraphRef(lexid s) {
    return lexid_TypeGraphRef_dict_get(UniverseDict, s);
}


//TODO: Write this better.
string get_type_name(TypeGraphRef r, nametable names) {
    lexid token = lexid_TypeGraphRef_dict_reverse_get(UniverseDict, r);
    return nametable_get(names, token);
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


