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
type_ref typeslot_get_ref(typeslot in);
polytype typeslot_get_type(typeslot in);
void print_typeslot(typeslot in, nametable names);
void print_typeslot_list(typeslot_dynarray in, nametable names);

//"Flattens" all type_refs within the dynarray recursively to their upper bounds
polytype_dynarray typeslot_dynarray_instantiate(typeslot_dynarray in);

int typeslot_trivial_eq(typeslot a, typeslot b);
int typeslot_dynarray_trivial_eq(typeslot_dynarray a, typeslot_dynarray b);

//Using the arguments from the given polytype, set bounds on type_refs in "to" to match.
//Returns "0" if there was a failure
int typeslot_dynarray_pour(polytype from, typeslot_dynarray to);

#endif
