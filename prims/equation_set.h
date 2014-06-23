#include "type_equation.h"
#include "nametable.h"

//Definition for a set of type equations

#ifndef EQN_SET_DEFINED
#define EQN_SET_DEFINED

typedef type_equation_dynarray equation_set;

typedef equation_set eqn_set;

eqn_set equation_set_init();
eqn_set equation_set_add(eqn_set s, type_equation eqn);
//Replace the referenced equation with a different one
eqn_set equation_set_replace(eqn_set s, type_equation* ref, type_equation val);

//Returns a reference to a polymorph equation associated with the given variable [if there is one]
type_equation* equation_set_getpoly(eqn_set s, type_ref var);

//Gets the first equation with the given kind
type_equation* equation_set_getkind(eqn_set s, type_expr_kind k);

//Gets all equations of a given kind
type_equation_ptr_dynarray equation_set_getall(eqn_set s, type_expr_kind k);

void print_type_equations(eqn_set s, nametable names);


//eqn_set equation_set_addequals(eqn_set s, type_ref other);

#endif
