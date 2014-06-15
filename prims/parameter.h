#include "type.h"
#include "../passes/lexer.h"

#ifndef PARAMETER_DEFINED
#define PARAMETER_DEFINED

typedef struct {
    TypeInfo type;
    lexid name; //Name (identifier)
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
static int parameter_ptr_type_is_known(parameter_ptr in) {
    return type_is_known(in->type);
}

static void print_parameter(parameter in, nametable names) {
    printf("( ");
    //print_type(in.type, names); TODO: FIXME!
    nametable_print(names, in.name);
    printf(" )");
    return;
}
static void print_parameter_ptr(parameter_ptr in, nametable names) {
    parameter toprint = *in;
    print_parameter(toprint, names);
    return;
}

DEFINE_DYNARRAY(parameter)


#endif
