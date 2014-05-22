#include "function.h"

#ifndef POLYMORPH_DEFINED
#define POLYMORPH_DEFINED

typedef struct {
    function_ptr_dynarray options; //List of functions that may work
    TypeInfo type; //most general type that can be returned
} polymorph;

#endif
