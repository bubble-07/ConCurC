#include "type.h"
#include "../passes/lexer.h"

#ifndef PARAMETER_DEFINED
#define PARAMETER_DEFINED

typedef struct {
    TypeInfo type;
    lexid name;
} parameter;

static void print_parameter(parameter in, string_dynarray backsymtable) {
    printf("( ");
    print_type(in.type);
    printf(to_cstring(backsymtable.begin[in.name.tokenval]));
    printf(" )");
    return;
}

DEFINE_DYNARRAY(parameter)


#endif
