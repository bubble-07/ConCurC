#include "type.h"
#include "../passes/lexer.h"

#ifndef PARAMETER_DEFINED
#define PARAMETER_DEFINED

typedef struct {
    TypeInfo type;
    lexid name;
} parameter;

//Define a pointer type for parameters.
typedef parameter* parameter_ptr;

const parameter_ptr parameter_ptr_lookup_failure = NULL;

//Define it so that parameter pointers are equal iff same address
static int parameter_ptr_eq(parameter_ptr one, parameter_ptr two) {
    return one == two;
}

static void print_parameter(parameter in, string_dynarray backsymtable) {
    printf("( ");
    print_type(in.type);
    printf(to_cstring(backsymtable.begin[in.name.tokenval]));
    printf(" )");
    return;
}

DEFINE_DYNARRAY(parameter)


#endif
