//Defines polytypes, which is a bit of a misnomer, since they can represent monotypes
//(such as when "argtypes" is empty. Nevertheless, generic container for a type specification.

#include "../libs/digraph.h"
#include "type_ref.h"

#ifndef POLYTYPE_DEFINED
#define POLYTYPE_DEFINED

typedef struct {
    noderef ref; //Stores a reference to the node in the type graph that stores the polymorph "stem"
    type_ref_dynarray argtypes;
} polytype;

void print_polytype(polytype in, nametable names);
polytype make_monotype(noderef in);

DEFINE_DYNARRAY(polytype)
#endif
