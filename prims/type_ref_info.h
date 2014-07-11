//NOTE: Corresponds to type_ref.c

#ifndef TYPE_REF_INFO_DEFINED
#define TYPE_REF_INFO_DEFINED

#include "equation_set.h"
#include "polytype.h"
#include "typeslot.h"

//The actual information is stored in a pointer from the "representative" node
//Using the following structure
typedef struct {
    polytype upperbound; //Represents the current upper bound on the type
    equation_set equations; //Represents the set of equations that apply to the type_ref
    int parametric; //Will be "0" if the type_ref in question is a simple type, and "1" if it could represent a parametric type
    int known; //Will be "0" until the type_ref is known, at which point this becomes "1"
} type_ref_info;

typedef type_ref_info* type_ref_info_ptr;
DEFINE_DYNARRAY(type_ref_info_ptr)

type_ref type_ref_add_equation(type_ref in, type_equation eqn);
type_ref type_ref_addpoly_eqn(type_ref in, polymorph_ptr poly, typeslot_dynarray args);
type_ref type_ref_addargpos_eqn(type_ref in, typeslot func, int pos);
type_ref type_ref_addapply_eqn(type_ref in, typeslot func, typeslot_dynarray args);

//Gets a pointer to the polymorph associated with a given type_ref (NULL if nonexistent)
type_ref make_known_parametric_type_ref(polytype in);

int type_ref_is_simple(type_ref in); 
int type_ref_is_parametric(type_ref in);
int type_ref_is_known(type_ref in);
int type_ref_is_unknown(type_ref in);

//Returns "1" if the two type_refs are pointing to the same information
int type_refs_same_class(type_ref one, type_ref two);

type_ref type_ref_setknown(type_ref in, int known);
type_ref type_ref_setparametric(type_ref in, int parametric);

polytype type_ref_getbound(type_ref in);
type_ref type_ref_setbound(type_ref in, polytype type);

type_ref type_ref_get_rep(type_ref in);

//Restricts the incoming type_ref to fall under the given type
//Returns "1" if the type was restricted, "0" otherwise
int type_ref_restrict(type_ref in, polytype info);

//Similar to restrict, but accepts a typeslot
int type_ref_constrain(type_ref in, typeslot info);

polymorph_ptr type_ref_getpoly(type_ref in);

type_equation_dynarray type_ref_get_equations(type_ref in);

type_ref_info_ptr_dynarray get_type_ref_info_list(type_ref_dynarray in);

type_ref_info_ptr_dynarray restore_type_ref_info_list(type_ref_info_ptr_dynarray infos, type_ref_dynarray dest);
#endif
