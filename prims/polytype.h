//Defines polytypes, which is a bit of a misnomer, since they can represent monotypes
//(such as when "argtypes" is empty. Nevertheless, generic container for a type specification.

#include "../libs/digraph.h"
#include "type_ref.h"

#include "typeslot.h"

#ifndef POLYTYPE_DEFINED

typedef struct {
    noderef ref; //Stores a reference to the node in the type graph that stores the polymorph "stem"
    typeslot_dynarray argtypes;
} polytype;

DEFINE_DYNARRAY(polytype)

#define POLYTYPE_DEFINED

typeslot get_polytype_arg(polytype in, int pos);
int polytype_numargs(polytype in);
void print_type(polytype in, nametable names);
polytype make_monotype(noderef in);
polytype make_bottom_type();
polytype make_polytype(noderef in, typeslot_dynarray args);
polytype make_unknown_type();
//Returns an identical copy of the incoming polytype
polytype copy_type(polytype in);
int polytype_trivial_eq(polytype one, polytype two);
int is_monotype(polytype in);
int is_bottom(polytype in); //Returns "true" if the given type is "bottom"

//Given the list of type references (representing free variables), mutates
//the list of type refs to point to the argument positions of the incoming polytype
int polytype_pour_args(polytype in, type_ref_dynarray refs);

//Returns true if the incoming polytype is "Any"
int is_any(polytype in);

polytype_dynarray polytype_get_subtypes(polytype in);

//Returns a type that represents the intersection of a and b
polytype intersect_types(polytype a, polytype b);

//Returns a type that represents the union of a and b (either)
polytype union_types(polytype a, polytype b);

//Attempts to simplify the incoming polytype (Elims extraneous "either"s)
polytype simplify_type(polytype in);

//Returns "true" if the two incoming types are disjoint [no common subtypes]
int types_are_disjoint(polytype a, polytype b);

//Returns "true" if the given types are equal (deep comparison)
int type_eq(polytype a, polytype b);

//Takes the src and fills all type_refs in dest with the info from src (matching)
//returns 0 for an unsuccessful match
int polytype_pour(polytype src, polytype dest);

#endif
