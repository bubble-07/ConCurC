#ifndef POLYMORPH_DEFINED
#define POLYMORPH_DEFINED

#include "function.h"

struct polymorph {
    function_ptr_dynarray options; //List of functions that may work
    int local; //Should be set to 1 if the polymorph in question is local (not in a function table) else, 0
};
typedef struct polymorph polymorph;

typedef polymorph* polymorph_ptr;

DEFINE_DYNARRAY(polymorph)
DEFINE_DYNARRAY(polymorph_ptr)

//Creates a non-local polymorph with no function options
polymorph make_empty_polymorph();

int polymorph_ptr_numoptions(polymorph_ptr in);

function_ptr_dynarray polymorph_ptr_get_options(polymorph_ptr in);

void polymorph_ptr_free(polymorph_ptr in);
            
//Gets a sum type of all possible types the parameter in position pos could be
//Assumes that the position pos does not accept a type variable
polytype polymorph_ptr_get_parameter_type(polymorph_ptr in, int pos);

//Gets the most general type that the polymorph would return for the given argument types
typeslot polymorph_ptr_get_return_type(polymorph_ptr in, typeslot_dynarray args);

//Same as make_empty_polymorph, but allocates on heap and returns a pointer
polymorph_ptr make_empty_polymorph_ptr();

polymorph_ptr make_empty_local_polymorph_ptr();

//Makes a local copy of the referenced polymorph
polymorph_ptr polymorph_ptr_copy(polymorph_ptr in);

//adds the given function pointer to the polymorph
polymorph add_to_polymorph(polymorph in, function_ptr f);

polymorph_ptr add_to_polymorph_ptr(polymorph_ptr in, function_ptr f);

//Rules out functions in the polymorph that do not fit with the given
//argument types. If at least one option was ruled out, return an entirely
//new, "forked" polymorph, deleting the old if it was local.
//Otherwise, return the input unchanged
//Assumes the polymorph_ptr passed in is non-null
polymorph_ptr polymorph_ptr_restrict(polymorph_ptr in, typeslot_dynarray args);


void print_polymorph_ptr(polymorph_ptr in, nametable names);

//Prints the name of the referred-to polymorph
void print_polymorph_ptr_name(polymorph_ptr in, nametable names);

static polymorph_ptr polymorph_ptr_lookup_failure = NULL;

int polymorph_ptr_eq(polymorph_ptr a, polymorph_ptr b);

#endif
