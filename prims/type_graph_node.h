#include "subtype_lattice.h"
#include "../libs/digraph.h"

#ifndef TYPE_GRAPH_NODE_DEFINED
#define TYPE_GRAPH_NODE_DEFINED

//Defines type graph nodes, and references to them

typedef struct {
    lexid name; //Stores the name of the type or polymorphic type constructor
    subtype_lattice* lattice; //Stores the subtype lattice of the given type [if polymorphic], or NULL if a monotype
} type_graph_node;

//Also define references to types within the context of graphs
typedef noderef TypeGraphRef;
static noderef TypeGraphRef_lookup_failure = -1; //define -1 to be failure

static type_graph_node make_type_graph_node(lexid name, subtype_lattice* lattice) {
    type_graph_node result;
    result.name = name;
    result.lattice = lattice;
    return result;
}

//Adds the given subtype to the subtype lattice of the incoming graph node
static type_graph_node type_graph_node_add_subtype(type_graph_node in, polytype sub) {
    if (in.lattice == NULL) {
        //Allocate a new one
    }
    in.lattice = lattice_add_subtype(in.lattice, typeslot_from_type(sub));
    return in;
}


static int TypeGraphRef_eq(TypeGraphRef a, TypeGraphRef b) {
    return a == b;
}

DEFINE_DYNARRAY(TypeGraphRef)


#endif
