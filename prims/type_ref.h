#include "type.h"

//Defines type_refs, which can either reference
//some type info, forward along to another type ref, or
//be completely empty. Used somewhat like type variables

typedef enum {
    type_ref_empty;
    type_ref_forwarding;
    type_ref_immediate;
} type_ref_kind;

typedef struct type_ref type_ref;

struct type_ref {
    type_ref* forward; //Could forward things
    TypeInfo type; //Or could be its own thing
    type_ref_kind kind; //Whatever the case, denote it by this
};

DEFINE_DYNARRAY(type_ref)

