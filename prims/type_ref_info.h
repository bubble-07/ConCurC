//NOTE: Corresponds to type_ref.c

#ifndef TYPE_REF_INFO_DEFINED
#define TYPE_REF_INFO_DEFINED

#include "type_equation.h"

//The actual information is stored in a pointer from the "representative" node
//Using the following structure
typedef struct {
    TypeInfo upperbound;
    type_equation_dynarray equations;
} type_ref_info;
#endif
