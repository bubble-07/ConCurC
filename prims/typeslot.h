#include "type_ref.h"
#include "nametable.h"

#ifndef TYPESLOT_DEFINED
#define TYPESLOT_DEFINED

//Represents a type "slot" which can either contain a type_ref
//Or a good 'ol polytype. 
//TODO: Integrate this into the definition of type_refs! (if possible)

typedef enum {
    typeslot_ref,
    typeslot_type
} typeslot_kind;

typedef struct {
    typeslot_kind kind;
    void* data; //Either it is a pointer to a polytype, or it stores a type_ref locally
} typeslot;

DEFINE_DYNARRAY(typeslot)

#include "polytype.h"

typeslot typeslot_from_ref(type_ref in);
typeslot typeslot_from_type(polytype in);
typeslot_kind typeslot_get_kind(typeslot in);

//Makes a typeslot that represents the bottom (empty) type
typeslot make_bottom_typeslot();

//Returns true if the given typeslot is "bottom"
int is_bottom_typeslot(typeslot in);

//Returns the union of two typeslots, with special rules for type_refs
typeslot union_typeslots(typeslot one, typeslot two);

type_ref typeslot_get_ref(typeslot in);
polytype typeslot_get_type(typeslot in);
void print_typeslot(typeslot in, nametable names);
void print_typeslot_list(typeslot_dynarray in, nametable names);

//Gets a "flattened" version of the typeslot
polytype typeslot_instantiate(typeslot in);

//"Flattens" all type_refs within the dynarray recursively to their upper bounds
polytype_dynarray typeslot_dynarray_instantiate(typeslot_dynarray in);

int typeslot_trivial_eq(typeslot a, typeslot b);
int typeslot_dynarray_trivial_eq(typeslot_dynarray a, typeslot_dynarray b);

int typeslot_pour(typeslot one, typeslot two);

//Takes a typeslot, and recursively changes all type_refs to their representative nodes
typeslot typeslot_dissociate(typeslot in);
typeslot_dynarray typeslot_dynarray_dissociate(typeslot_dynarray in);

#endif
