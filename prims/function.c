#include "function.h"
#include "type_ref_info.h"

//Gets the type of the parameter in the given position
//Returns empty type if outside of bounds
typeslot function_ptr_get_parameter_type(function_ptr in, int pos) {
    pos = pos - 1; //Do this so "1" corresponds to the first element in the array of params
    if (in->params.size > pos) {
        return get_parameter_ptr_type(in->params.begin[pos]);
    }
    return typeslot_from_type(make_bottom_type());
}
//Returns the return type of the given function for the given args
typeslot function_ptr_get_return_type(function_ptr in, typeslot_dynarray args) {
    
    //Record all bounds associated with all type_refs in the function signature
    type_ref_info_ptr_dynarray bounds = get_type_ref_info_list(in->type_vars);
    
    int i;
    int success = 1; //Will be 0 if there was a failure in argument matching
    for (i=0; i < in->params.size; i++) {
        //"pour" each argument in to the mold given 
        //Note: parameter_ptr_pour fails if the incoming type is too general,
        //which is exactly what we want for checking left-to-right
        success = success && parameter_ptr_pour(args.begin[i], in->params.begin[i]);
    }

    //Generate the result by turning all type_refs in the return type into type_refs
    //that reference the representative nodes of type_refs so we can safely sever the first
    //level of links when we reset the types [for future invocations]
    typeslot result = typeslot_dissociate(in->ret_type);
    
    //Reset the bounds on all of the type_refs the function has in its signature
    restore_type_ref_info_list(bounds, in->type_vars);

    if (success == 1) { 
        //If we were able to apply the function
        //Return the result of 
        return result;
    }
    //Otherwise, just say that the function can return anything
    return typeslot_from_type(make_unknown_type());
}

function_ptr function_ptr_set_return_type(function_ptr in, typeslot t) {
    in->ret_type = t;
    return in;
}

//Returns true if the given argument types stand a chance of being accepted, 0 otherwise
//Right now, do this the REALLY dumb way
int function_ptr_accepts(function_ptr in, typeslot_dynarray args) {
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
