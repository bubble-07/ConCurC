//Defines a subtype lattice, which is a basic representation
//of the subtypes of a given type. 

//For now, only do single-head subtype lattices [that is,
//no "Mat Int A" and "Mat Float A"], with no branching past
//the first level and no equality constraints [Ex: SquareMat n <: Mat n n]

#include "polytype.h"

#ifndef SUBTYPE_LATTICE_DEFINED
#define SUBTYPE_LATTICE_DEFINED
typedef struct {
    type_ref_dynarray head; //The list of parameter types to match
    polytype_dynarray subtypes; //Set of all subtypes that derive from the head
} subtype_lattice;

typedef subtype_lattice sub_lat; //Shorthand

//Gets the subtypes of the incoming type that are enumerated in the lattice
TypeInfo lattice_get_subtypes(polytype in, subtype_lattice* lattice);

subtype_lattice* make_monotype_lattice();
subtype_lattice* make_polytype_lattice(type_ref_dynarray in);

subtype_lattice* lattice_add_subtype(subtype_lattice* in, polytype subtype);
#endif
