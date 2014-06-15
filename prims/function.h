#include "../libs/dynarray.h"
#include "parameter.h"
#include "cell_tree.h"
#include "lexid.h"
#include "nametable.h"

#ifndef FUNCTION_DEFINED
#define FUNCTION_DEFINED

typedef struct {
    lexid name; //Name of the function (identifier)
    type_ref ret_type; //Return type of function
    cell_tree body; //Body of the function
    parameter_ptr_dynarray params; //Parameters of the function
} function;



typedef function* function_ptr;

DEFINE_DYNARRAY(function_ptr)

DEFINE_DYNARRAY(function)


//Gets the type of the parameter in the given position
//Returns null if outside of bounds
static type_ref function_ptr_get_parameter_type(function_ptr in, int pos) {
    if (in->params.size > pos) {
        return get_parameter_type(*in->params.begin[pos]);
    }
    return NULL;
}
//Returns the return type of the given function
static type_ref function_ptr_get_return_type(function_ptr in) {
    return in->ret_type;
}

static function_ptr function_ptr_set_return_type(function_ptr in, type_ref t) {
    in->ret_type = t;
    return in;
}

static void print_function(function in, nametable names) {
    printf("Name: "); 
    nametable_print(names, in.name);
    printf(" Return type: ");
    //print_type(in.ret_type, names); TODO: FIXME!
    printf(" Parameters: ");
    int i;
    for (i = 0; i < in.params.size; i++) {
        print_parameter_ptr(in.params.begin[i], names);
    }
    printf("\nBody:");
    print_cell_tree(in.body, names);
    return;
}

static void print_function_ptr_name(function_ptr in, nametable names) {
    nametable_print(names, in->name);
    return;
}
    
#endif
