#include "../libs/dynarray.h"
#include "parameter.h"

#ifndef LAMBDA_DEFINED
#define LAMBDA_DEFINED


//NOTE: The body of a lambda expression comes underneath the
//lambda cell in a cell tree
//Why? Makes nested scopes during type inference easier to deal with

typedef struct {
    TypeInfo retType; //Return type
    parameter_dynarray params; //Parameters
} lambda;

typedef lambda* lambda_ptr;

DEFINE_DYNARRAY(lambda_ptr)
DEFINE_DYNARRAY(lambda)

#endif
