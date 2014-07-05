//Defines polytypes, which is a bit of a misnomer, since they can represent monotypes
//(such as when "argtypes" is empty. Nevertheless, generic container for a type specification.

#include "../libs/digraph.h"
#include "type_ref.h"

#ifndef POLYTYPE_DEFINED

typedef struct {
    noderef ref; //Stores a reference to the node in the type graph that stores the polymorph "stem"
    type_ref_dynarray argtypes;
} polytype;

DEFINE_DYNARRAY(polytype)

#define POLYTYPE_DEFINED

#ifndef TYPEINFO_DEFINED
#include "typeinfo.h"
#endif

void print_polytype(polytype in, nametable names);
polytype make_monotype(noderef in);
polytype make_polytype(noderef in, type_ref_dynarray args);
int polytype_trivial_eq(polytype one, polytype two);
int is_monotype(polytype in);

//Returns true if the incoming polytype is "Any"
int is_any(polytype in);

TypeInfo polytype_get_subtypes(polytype in);

void polytype_pour_args(polytype one, polytype two);

#endif
