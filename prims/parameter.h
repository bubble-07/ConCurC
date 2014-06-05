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

static const parameter_ptr parameter_ptr_lookup_failure = NULL;

//Define it so that parameter pointers are equal iff same address
static int parameter_ptr_eq(parameter_ptr one, parameter_ptr two) {
    return one == two;
}

static TypeInfo get_parameter_type(parameter in) {
    return in.type;
}

static TypeInfo get_parameter_ptr_type(parameter_ptr in) {
    return in->type;
}
static parameter_ptr set_parameter_ptr_type(parameter_ptr in, TypeInfo i) {
    in->type = i;
    return in;
}

static void print_parameter(parameter in, string_dynarray backsymtable) {
    printf("( ");
    print_type(in.type, backsymtable);
    printf(to_cstring(backsymtable.begin[in.name.tokenval]));
    printf(" )");
    return;
}
static void print_parameter_ptr(parameter_ptr in, string_dynarray backsymtable) {
    parameter toprint = *in;
    print_parameter(toprint, backsymtable);
    return;
}

DEFINE_DYNARRAY(parameter)


#endif
