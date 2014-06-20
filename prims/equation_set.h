#include "type_equation.h"
#include "nametable.h"

//Definition for a set of type equations

#ifndef EQN_SET_DEFINED
#define EQN_SET_DEFINED

//We're going to be dealing with a dictionary of pointers to dynarrays
//Indexed by type_refs as keys. 
typedef type_equation_dynarray* eqn_listref;

DEFINE_DYNARRAY(eqn_listref)

//Equal iff pointers equal
static int eqn_listref_eq(eqn_listref one, eqn_listref two) {
    return one == two;
}

static eqn_listref eqn_listref_lookup_failure = NULL;

DEFINE_DICT(type_ref, eqn_listref)
DEFINE_GET_ALL_VALS(type_ref, eqn_listref)

typedef struct {
    type_ref_eqn_listref_dict dict;
} equation_set;

typedef equation_set eqn_set;

eqn_set equation_set_init();
eqn_set equation_set_add(eqn_set s, type_equation eqn);
eqn_set equation_set_addpoly(eqn_set s, type_ref var, polymorph_ptr poly, type_ref_dynarray args);
eqn_set equation_set_addargpos(eqn_set s, type_ref var, type_ref func, int pos);
eqn_set equation_set_addapply(eqn_set s, type_ref var, type_ref func, type_ref_dynarray args);
eqn_set equation_set_addsubtype(eqn_set s, type_ref var, type_ref super);

//Returns a reference to a polymorph equation associated with the given variable [if there is one]
type_equation* equation_set_getpoly(eqn_set s, type_ref var);


//Gets a pointer to the dynarray storing all equations for a given type_ref
eqn_listref equation_set_getvareqns(eqn_set s, type_ref var);

//Gets the first equation with the given kind corresponding to the given type var
type_equation* equation_set_getkind(eqn_set s, type_ref var, type_expr_kind k);

//Gets all equations of a given type
type_equation_ptr_dynarray equation_set_getall(eqn_set s, type_expr_kind k);

void print_type_equations(eqn_set s, nametable names);


//eqn_set equation_set_addequals(eqn_set s, type_ref var, type_ref other);

#endif
