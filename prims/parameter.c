#include "parameter.h"
#include "type_ref_info.h"

parameter_ptr parameter_ptr_make(typeslot type, lexid name) {
    parameter_ptr result = memalloc(sizeof(parameter));
    result->type = type;
    result->name = name;
    return result;
}

//Define it so that parameter pointers are equal iff same address
int parameter_ptr_eq(parameter_ptr one, parameter_ptr two) {
    return one == two;
}

typeslot get_parameter_ptr_type(parameter_ptr in) {
    return in->type;
}

//Assumes that the given parameter has a non-variable type
polytype get_parameter_ptr_bound(parameter_ptr in) {
    return typeslot_instantiate(in->type);
}

parameter_ptr set_parameter_ptr_type(parameter_ptr in, typeslot i) {
    in->type = i;
    return in;
}

//Returns "true" if the parameter's bounding type and the other's bounding type are not disjoint
int parameter_ptr_accepts(parameter_ptr in, typeslot val) {
    return !types_are_disjoint(typeslot_instantiate(in->type), typeslot_instantiate(val));
}

void print_parameter(parameter in, nametable names) {
    printf("( ");
    //print_type(in.type, names);// TODO: FIXME!
    nametable_print(names, in.name);
    printf(" )");
    return;
}
void print_parameter_ptr(parameter_ptr in, nametable names) {
    parameter toprint = *in;
    print_parameter(toprint, names);
    return;
}

int parameter_ptr_pour(typeslot src, parameter_ptr dest) {
    return typeslot_pour(src, dest->type);
}
