#include "equation_set.h"

#ifndef TYPE_RULES_DEFINED
#define TYPE_RULES_DEFINED


typedef struct {
    equation_set eqns; //The (modified) set of equations
    int active; //Indicates whether a rule was applied to the equations
} rule_app_result; //Result of a rule application

rule_app_result rule_app_result_init(equation_set eqns);
rule_app_result for_every(type_expr_kind k, equation_set eqns, rule_app_result (*f)(type_equation*, equation_set));
rule_app_result with_polymorph(type_equation* other_eqn, type_ref func_type, equation_set eqns, rule_app_result (*f)(type_equation*, type_equation*, equation_set));


#endif
