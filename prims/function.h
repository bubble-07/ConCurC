#include "../libs/dynarray.h"
#include "parameter.h"
#include "lexid.h"
#include "nametable.h"
#include "cell_tree.h"
#include "../prims/typeslot.h"

#ifndef FUNCTION_DEFINED
#define FUNCTION_DEFINED

typedef struct {
    lexid name; //Name of the function (identifier)
    typeslot ret_type; //Return type of function
    cell_tree body; //Body of the function
    parameter_ptr_dynarray params; //Parameters of the function
    type_ref_dynarray type_vars; //List of all type vars that occur in the function signature
} function;


typedef function* function_ptr;

DEFINE_DYNARRAY(function_ptr)

DEFINE_DYNARRAY(function)


typeslot function_ptr_get_parameter_type(function_ptr in, int pos);
typeslot function_ptr_get_return_type(function_ptr in, typeslot_dynarray args);
function_ptr function_ptr_set_return_type(function_ptr in, typeslot t);
int function_ptr_accepts(function_ptr in, typeslot_dynarray args);
void print_function(function in, nametable names);
void print_function_ptr_name(function_ptr in, nametable names);
    
#endif
