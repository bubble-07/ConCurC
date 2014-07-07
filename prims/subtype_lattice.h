//Defines a subtype lattice, which is a basic representation
//of the subtypes of a given type. 

//For now, only do single-head subtype lattices [that is,
//no "Mat Int A" and "Mat Float A"], with no branching past
//the first level and no equality constraints [Ex: SquareMat n <: Mat n n]

//TODO: Check that we're always provided the right number of arguments!

#include "polytype.h"
#include "typeslot.h"

#ifndef SUBTYPE_LATTICE_DEFINED
#define SUBTYPE_LATTICE_DEFINED
//Implemented using two parallel lists, one of possible supertypes to match, and
//the other of resultant subtypes
typedef struct {
    polytype_dynarray head; //The list of possible incoming types to match
    typeslot_dynarray subtypes; //Set of all subtypes that derive from the head
} subtype_lattice;

typedef subtype_lattice sub_lat; //Shorthand

//Gets the subtypes of the incoming type that are enumerated in the lattice
polytype_dynarray lattice_get_subtypes(polytype in, subtype_lattice* lattice);

subtype_lattice* make_lattice();

subtype_lattice* lattice_add_subtype(subtype_lattice* in, typeslot subtype, polytype supertype);
#endif
