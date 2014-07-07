//NOTE: Corresponds to type_ref.c

#ifndef TYPE_REF_INFO_DEFINED
#define TYPE_REF_INFO_DEFINED

#include "equation_set.h"
#include "polytype.h"

//The actual information is stored in a pointer from the "representative" node
//Using the following structure
typedef struct {
    polytype upperbound;
    equation_set equations;
} type_ref_info;

type_ref type_ref_add_equation(type_ref in, type_equation eqn);
type_ref type_ref_addpoly_eqn(type_ref in, polymorph_ptr poly, type_ref_dynarray args);
type_ref type_ref_addargpos_eqn(type_ref in, type_ref func, int pos);
type_ref type_ref_addapply_eqn(type_ref in, type_ref func, type_ref_dynarray args);

type_ref make_known_type_ref(polytype in); //Makes a new type ref with a bounded type
//Gets a pointer to the polymorph associated with a given type_ref (NULL if nonexistent)

polytype type_ref_getbound(type_ref in);
type_ref type_ref_setbound(type_ref in, polytype type);

//Restricts the incoming type_ref to fall under the given type
//Returns "1" if the type was restricted, "0" otherwise
int type_ref_restrict(type_ref in, polytype info);

polymorph_ptr type_ref_getpoly(type_ref in);

type_equation_dynarray type_ref_get_equations(type_ref in);

#endif
