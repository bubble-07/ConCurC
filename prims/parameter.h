#include "../passes/lexer.h"

#ifndef PARAMETER_DEFINED
#define PARAMETER_DEFINED

#include "type_ref.h"

typedef struct {
    type_ref type;
    lexid name; //Name (identifier)
} parameter;

//Define a pointer type for parameters.
typedef parameter* parameter_ptr;

static parameter_ptr parameter_ptr_make(type_ref type, lexid name) {
    parameter_ptr result = memalloc(sizeof(parameter));
    result->type = type;
    result->name = name;
    return result;
}

static const parameter_ptr parameter_ptr_lookup_failure = NULL;

//Define it so that parameter pointers are equal iff same address
static int parameter_ptr_eq(parameter_ptr one, parameter_ptr two) {
    return one == two;
}

static type_ref get_parameter_ptr_type_ref(parameter_ptr in) {
    return in->type;
}

//Assumes that the given parameter has a non-variable type
static TypeInfo get_parameter_ptr_bound(parameter_ptr in) {
    return type_ref_getbound(in->type);
}

static parameter_ptr set_parameter_ptr_type(parameter_ptr in, type_ref i) {
    in->type = i;
    return in;
}

static void print_parameter(parameter in, nametable names) {
    printf("( ");
    //print_type(in.type, names);// TODO: FIXME!
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
DEFINE_DYNARRAY(parameter_ptr)


#endif
