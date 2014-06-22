#include "../libs/dynarray.h"

#ifndef LAMBDA_DEFINED
#define LAMBDA_DEFINED

#include "parameter.h"

//NOTE: The body of a lambda expression comes underneath the
//lambda cell in a cell tree
//Why? Makes nested scopes during type inference easier to deal with

typedef struct {
    type_ref ret_type; //Return type
    parameter_ptr_dynarray params; //Parameters
} lambda;

typedef lambda* lambda_ptr;

DEFINE_DYNARRAY(lambda_ptr)
DEFINE_DYNARRAY(lambda)

#endif

