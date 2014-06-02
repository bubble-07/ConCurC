#include "../libs/dynarray.h"
#include "parameter.h"
#include "cell.h"

#ifndef FUNCTION_DEFINED
#define FUNCTION_DEFINED

typedef struct {

    TypeInfo retType; //Return type of function
    cell_tree body; //Body of the function
    parameter_dynarray params; //Parameters of the function
} function;

static void print_function(function in, string_dynarray backsymtable) {
    printf("Return type: ");
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



typedef function* function_ptr;

DEFINE_DYNARRAY(function_ptr)

DEFINE_DYNARRAY(function)

#endif
