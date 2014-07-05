//Defines the type graph, which is a directed graph that represents
//the subtyping relation exactly on monotypes (FIXME: This is only partially correct with the current implementation!)
//and represents "possible subtyping" on polytypes (used to deal with "constraint explosion")

#include "../libs/dynstring.h"
#include "../libs/dict.h"
#include "../prims/lexid.h"
#include "../prims/nametable.h"


#include "../libs/digraph.h"
#include "type_graph_node.h"
#include "polytype.h"

#ifdef TYPE_GRAPH_DEFINED
extern Type_graph UniverseGraph; //Give access to the type universe
extern lexid_TypeGraphRef_dict UniverseDict;
extern polytype Top;
extern TypeGraphRef Mono; //Defines the bridge from polytypes to monotypes
#endif

#ifndef TYPE_GRAPH_DEFINED
#define TYPE_GRAPH_DEFINED

typedef type_graph_node Type;

DEFINE_GRAPH(Type)

//Define a dictionary allowing us to look up nodes in the dictionary
//based upon the name of a type
DEFINE_DICT(lexid, TypeGraphRef)



DEFINE_REVERSE_LOOKUP(lexid, TypeGraphRef)

//Define a global type universe containing everything we want
Type_graph UniverseGraph;
//And define a global dictionary for looking up types
lexid_TypeGraphRef_dict UniverseDict;
//Give a reference to the "Any" or "Top" type
polytype Top;
TypeGraphRef Mono;

void init_type_universe();
TypeGraphRef get_TypeGraphRef(lexid s);
string get_type_name(TypeGraphRef r, nametable names);
void print_TypeGraphRef(TypeGraphRef r, nametable names);
type_graph_node get_graph_node(TypeGraphRef r);
//Returns "true" if it's possible for a to subtype b
int Type_graph_possiblesubtype(Type_graph in, polytype a, polytype b);

#endif
