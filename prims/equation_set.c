#include "equation_set.h"

eqn_set equation_set_init() {
    eqn_set result;
    result.dict = type_ref_eqn_listref_dict_init(10);
    return result;
}

eqn_listref equation_set_getvareqns(eqn_set s, type_ref var) {
    return type_ref_eqn_listref_dict_get(s.dict, var);
}

eqn_set equation_set_add(eqn_set s, type_equation eqn) {
    eqn_listref target_list = equation_set_getvareqns(s, eqn.var);
    if (target_list == eqn_listref_lookup_failure) {

        //Need to add a new entry and a new list for the variable!
        eqn_listref new_list = memalloc(sizeof(type_equation_dynarray));
        //Populate it with the equation
        *new_list = type_equation_dynarray_add(type_equation_dynarray_make(1), eqn);
        //Add it to the dictionary
        s.dict = type_ref_eqn_listref_dict_quickadd(s.dict, eqn.var, new_list);
    }
    else {
        //List must already be there!
        //Just add to the dynarray stationed there
        *target_list = type_equation_dynarray_add(*target_list, eqn);
    }
    return s;
}

eqn_set equation_set_addpoly(eqn_set s, type_ref var, polymorph_ptr poly, type_ref_dynarray args) {
    return equation_set_add(s, make_poly_eqn(var, poly, args));
}

eqn_set equation_set_addargpos(eqn_set s, type_ref var, type_ref func, int pos) {
    return equation_set_add(s, make_argpos_eqn(var, pos, func));
}

eqn_set equation_set_addapply(eqn_set s, type_ref var, type_ref func, type_ref_dynarray args) {
    return equation_set_add(s, make_apply_eqn(var, func, args));
}

eqn_set equation_set_addsubtype(eqn_set s, type_ref var, type_ref super) {
    return equation_set_add(s, make_subtype_eqn(var, super));
}

type_equation* equation_set_getkind(eqn_set s, type_ref var, type_expr_kind k) {
    eqn_listref var_eqns = equation_set_getvareqns(s, var);
    if (var_eqns == NULL) {
        return NULL;
    }
    int i;
    for (i=0; i < var_eqns->size; i++) {
        if (var_eqns->begin[i].expr_kind == k) {
            return &var_eqns->begin[i];
        }
    }
    return NULL; //couldn't find it
}



type_equation_ptr_dynarray equation_set_getall(eqn_set s, type_expr_kind k) {
    type_equation_ptr_dynarray result = type_equation_ptr_dynarray_make(1);

    eqn_listref_dynarray eqnlists = type_ref_eqn_listref_dict_get_all_values(s.dict);
    int i;
    int j;
    for (i=0; i < eqnlists.size; i++) {
        type_equation_dynarray eqns = *eqnlists.begin[i];
        for (j=0; j < eqns.size; j++) {
            //If the kind is what we're looking for
            if (eqns.begin[j].expr_kind == k) {
                //Add it!
                result = type_equation_ptr_dynarray_add(result, &eqns.begin[j]);
            }
        }
    }
    return result;
}
        

void print_type_equations(eqn_set s, nametable names) {
    eqn_listref_dynarray paramlists = type_ref_eqn_listref_dict_get_all_values(s.dict);
    int i;
    int j;
    for (i=0; i < paramlists.size; i++) {
        type_equation_dynarray eqns = *paramlists.begin[i];
        for (j=0; j < eqns.size; j++) {
            print_type_equation(eqns.begin[j], names);
        }
    }
    return;
}
    

