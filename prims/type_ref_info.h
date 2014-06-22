//NOTE: Corresponds to type_ref.c

#ifndef TYPE_REF_INFO_DEFINED
#define TYPE_REF_INFO_DEFINED

#include "equation_set.h"

//The actual information is stored in a pointer from the "representative" node
//Using the following structure
typedef struct {
    TypeInfo upperbound;
    equation_set equations;
} type_ref_info;
#endif
