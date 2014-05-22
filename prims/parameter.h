#include "type.h"

#ifndef PARAMETER_DEFINED
#define PARAMETER_DEFINED

typedef struct {
    TypeInfo type;
} parameter;

DEFINE_DYNARRAY(parameter)


#endif
