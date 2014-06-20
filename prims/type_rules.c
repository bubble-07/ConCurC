#include "type_rules.h"

//Initializes an inactive rule application result
rule_app_result rule_app_result_init(equation_set eqns) {
    rule_app_result result;
    result.active = 0;
    result.eqns = eqns;
    return result;
}

//Finds every expression of a given kind and applies the given rule to it
rule_app_result for_every(type_expr_kind k, equation_set s, 
                          rule_app_result (*f)(type_equation*, equation_set)) {
    rule_app_result tmp_result;
    int total_activity = 0;

    //Get all of the equations with the given kind
    type_equation_ptr_dynarray filtered = equation_set_getall(s, k);

    int i;
    for (i=0; i < filtered.size; i++) {
        tmp_result = f(filtered.begin[i], s);
        //Some plumbing
        s = tmp_result.eqns;
        //If any of the applications were "active", the whole thing is
        total_activity = total_activity || tmp_result.active;
    }
    //Return our final result
    tmp_result.eqns = s;
    tmp_result.active = total_activity;
    return tmp_result;
}

//Finds the polymorph corresponding to the given type var and applies a rule to it
//The rule is specified as a function accepting the other equation and the polymorph equation
rule_app_result with_polymorph(type_equation* other_eqn, type_ref func_type, equation_set eqns,
                               rule_app_result (*f)(type_equation*, type_equation*, equation_set)) {

    type_equation* poly_eqn = equation_set_getkind(eqns, func_type, is_polymorph_kind);
    if (poly_eqn != NULL) {
        return f(other_eqn, poly_eqn, eqns);
    }

    //Otherwise, return empty-handed TODO: Error?
    return rule_app_result_init(eqns);
}


