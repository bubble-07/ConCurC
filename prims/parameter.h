#include "../passes/lexer.h"

#ifndef PARAMETER_DEFINED
#define PARAMETER_DEFINED

#include "typeslot.h"
#include "polytype.h"

typedef struct {
    typeslot type;
    lexid name; //Name (identifier)
} parameter;

//Define a pointer type for parameters.
typedef parameter* parameter_ptr;

static const parameter_ptr parameter_ptr_lookup_failure = NULL;

parameter_ptr parameter_ptr_make(typeslot type, lexid name);
int parameter_ptr_eq(parameter_ptr one, parameter_ptr two);
typeslot get_parameter_ptr_type(parameter_ptr in);
polytype get_parameter_ptr_bound(parameter_ptr in);
parameter_ptr set_parameter_ptr_type(parameter_ptr in, typeslot i);
int parameter_ptr_accepts(parameter_ptr in, typeslot val);
void print_parameter(parameter in, nametable names);
void print_parameter_ptr(parameter_ptr in, nametable names);

DEFINE_DYNARRAY(parameter)
DEFINE_DYNARRAY(parameter_ptr)


#endif
