#include "type_ref.h"

#ifndef TYPE_EQN_DEFINED
#define TYPE_EQN_DEFINED

#include "polymorph.h"

//Define type equations and supporting architecture

typedef struct {
    polymorph_ptr poly;
    typeslot_dynarray args;
} is_polymorph;
//Defines a structure for the RH side of an equation
//that says a given type is a polymorphic function
//with the given argument types

typedef struct {
    int pos;
    typeslot func;
} is_in_pos;
//Defines a RH side that says a given type must fit
//in the specified position of a given function type

typedef struct {
    typeslot func;
    typeslot_dynarray args;
} is_result_of;
//Says that a given type is the result returned from
//applying func to args

typedef union {
    is_polymorph is_polymorph_entry;
    is_in_pos is_in_pos_entry;
    is_result_of is_result_of_entry;
} type_expr;
//Poor-man's ADT for possible RH sides of type equations

typedef enum {
    is_polymorph_kind,
    is_in_pos_kind,
    is_result_of_kind,
} type_expr_kind;
//Corresponding enum tags for possible RH sides


typedef struct {
    type_expr expr;
    type_expr_kind expr_kind;
} type_equation;

typedef type_equation* type_equation_ptr;

type_equation make_poly_eqn(polymorph_ptr p, typeslot_dynarray args);
type_equation make_argpos_eqn(int pos, typeslot functype);
type_equation make_apply_eqn(typeslot functype, typeslot_dynarray args);

DEFINE_DYNARRAY(type_equation)
DEFINE_DYNARRAY(type_equation_ptr)

is_polymorph get_poly_RH(type_equation in);
is_in_pos get_argpos_RH(type_equation in);
is_result_of get_apply_RH(type_equation in);

type_equation print_type_equation(type_equation in, nametable names);

#endif
