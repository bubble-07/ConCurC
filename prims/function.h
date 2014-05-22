#include "parameter.h"

#ifndef FUNCTION_DEFINED
#define FUNCTION_DEFINED

typedef struct {

    TypeRef retType; //Return type of function
    cell_tree body; //Body of the function
    parameter_dynarray params; //Parameters of the function
} function;

typedef function* function_ptr;

DEFINE_DYNARRAY(function_ptr)

#endif
