#include "../libs/dynarray.h"
#include "parameter.h"
#include "cell_tree.h"
#include "lexid.h"

#ifndef FUNCTION_DEFINED
#define FUNCTION_DEFINED

typedef struct {
    lexid name; //Name of the function (identifier)
    TypeInfo retType; //Return type of function
    cell_tree body; //Body of the function
    parameter_dynarray params; //Parameters of the function
} function;



typedef function* function_ptr;

DEFINE_DYNARRAY(function_ptr)

DEFINE_DYNARRAY(function)


//Gets the type of the parameter in the given position
//Returns an empty type if outside of bounds
static TypeInfo function_ptr_get_parameter_type(function_ptr in, int pos) {
    if (in->params.size > pos) {
        return get_parameter_type(in->params.begin[pos]);
    }
    return make_empty_type();
}

static void print_function(function in, string_dynarray backsymtable) {
    printf("Name: "); 
    printf(to_cstring(backsymtable.begin[in.name.tokenval]));
    printf(" Return type: ");
    print_type(in.retType, backsymtable);
    printf(" Parameters: ");
    int i;
    for (i = 0; i < in.params.size; i++) {
        print_parameter(in.params.begin[i], backsymtable);
    }
    printf("\nBody:");
    print_cell_tree(in.body, backsymtable);
    return;
}
#endif
