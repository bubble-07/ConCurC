#include "function.h"
#include "type_ref_info.h"

//Gets the type of the parameter in the given position
//Returns empty type if outside of bounds
TypeInfo function_ptr_get_parameter_type(function_ptr in, int pos) {
    pos = pos - 1; //Do this so "1" corresponds to the first element in the array of params
    if (in->params.size > pos) {
        return get_parameter_ptr_bound(in->params.begin[pos]);
    }
    return make_empty_type();
}
//Returns the return type of the given function
TypeInfo function_ptr_get_return_type(function_ptr in) {
    return type_ref_getbound(in->ret_type);
}

function_ptr function_ptr_set_return_type(function_ptr in, type_ref t) {
    in->ret_type = t;
    return in;
}

//Returns true if the given argument types stand a chance of being accepted, 0 otherwise
//Right now, do this the REALLY dumb way
int function_ptr_accepts(function_ptr in, type_ref_dynarray args) {
    if (in->params.size != args.size) {
        return 0; //TODO: support currying!
    }
    int i;
    for (i=0; i < args.size; i++) {
        if (!parameter_ptr_accepts(in->params.begin[i], args.begin[i])) {
            return 0;
        }
    }
    return 1;
}

void print_function(function in, nametable names) {
    printf("Name: "); 
    nametable_print(names, in.name);
    printf(" Return type: ");
    //print_type(in.ret_type, names); TODO: FIXME!
    printf(" Parameters: ");
    int i;
    for (i = 0; i < in.params.size; i++) {
        print_parameter_ptr(in.params.begin[i], names);
    }
    printf("\nBody:");
    print_cell_tree(in.body, names);
    return;
}

void print_function_ptr_name(function_ptr in, nametable names) {
    nametable_print(names, in->name);
    return;
}
