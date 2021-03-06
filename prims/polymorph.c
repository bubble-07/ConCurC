#include "polymorph.h"

//Creates a non-local polymorph with no function options
polymorph make_empty_polymorph() {
    polymorph result;
    result.options = function_ptr_dynarray_make(1);
    result.local = 0;
    return result;
}

int polymorph_ptr_numoptions(polymorph_ptr in) {
    return in->options.size;
}

function_ptr_dynarray polymorph_ptr_get_options(polymorph_ptr in) {
    return in->options;
}

void polymorph_ptr_free(polymorph_ptr in) {
    function_ptr_dynarray_free(in->options);
    return;
}
            
//Gets a sum type of all possible types the parameter in position pos could be
//Assumes that the position pos does not accept a type variable
polytype polymorph_ptr_get_parameter_type(polymorph_ptr in, int pos) {
    function_ptr_dynarray options = polymorph_ptr_get_options(in);
    polytype result = make_bottom_type();
    int i;
    for (i=0; i < options.size; i++) {
        //Add more options for what the type can be
        result = union_types(result, typeslot_instantiate(function_ptr_get_parameter_type(options.begin[i], pos)));
    }
    return result;
}

//Gets the most general type that the polymorph would return for the given argument types
typeslot polymorph_ptr_get_return_type(polymorph_ptr in, typeslot_dynarray args) {
    function_ptr_dynarray options = polymorph_ptr_get_options(in);
    typeslot result = make_bottom_typeslot();
    int i;
    for (i=0; i < options.size; i++) {
        result = union_typeslots(result, function_ptr_get_return_type(options.begin[i], args));
    }
    return result;
}

//Same as make_empty_polymorph, but allocates on heap and returns a pointer
polymorph_ptr make_empty_polymorph_ptr() {
    polymorph_ptr result = memalloc(sizeof(polymorph));
    *result = make_empty_polymorph();
    return result;
}

polymorph_ptr make_empty_local_polymorph_ptr() {
    polymorph_ptr result = make_empty_polymorph_ptr();
    result->local = 1;
    return result;
}

//Makes a local copy of the referenced polymorph
polymorph_ptr polymorph_ptr_copy(polymorph_ptr in) {
    polymorph_ptr result = memalloc(sizeof(polymorph));
    *result = *in;
    result->local = 1;
    return result;
}


//adds the given function pointer to the polymorph
polymorph add_to_polymorph(polymorph in, function_ptr f) {
    //TODO: Update the most general type returned when you do this!
    in.options = function_ptr_dynarray_add(in.options, f);
    return in;
}

polymorph_ptr add_to_polymorph_ptr(polymorph_ptr in, function_ptr f) {
    in->options = function_ptr_dynarray_add(in->options, f);
    return in;
}

//Rules out functions in the polymorph that do not fit with the given
//argument types. If at least one option was ruled out, return an entirely
//new, "forked" polymorph, deleting the old if it was local.
//Otherwise, return the input unchanged
//Assumes the polymorph_ptr passed in is non-null
polymorph_ptr polymorph_ptr_restrict(polymorph_ptr in, typeslot_dynarray args) {
    //Tentatively allocate a new polymorph pointer as a result
    polymorph_ptr result = make_empty_local_polymorph_ptr();

    int i;
    for (i=0; i < in->options.size; i++) {
        if (function_ptr_accepts(in->options.begin[i], args)) {
            //If the current function accepts the parameters, add it to the result
            add_to_polymorph_ptr(result, in->options.begin[i]);
        }
    }
    
    //If the result is restricted at all
    if (polymorph_ptr_numoptions(result) < polymorph_ptr_numoptions(in)) {
        //Return it, and free the old one if it is local
        if (in->local == 1) {
            polymorph_ptr_free(in);
        }
        return result;
    }
    //Otherwise, free it and return the input
    polymorph_ptr_free(result);
    return in;
}


void print_polymorph_ptr(polymorph_ptr in, nametable names) {
    int i;
    for (i=0; i < in->options.size; i++) {
        print_function(*(in->options.begin[i]), names);
        printf("\n");
    }
    return;
}

//Prints the name of the referred-to polymorph
void print_polymorph_ptr_name(polymorph_ptr in, nametable names) {
    if (in->options.size > 0) {
        print_function_ptr_name(in->options.begin[0], names);
    }
    return; //TODO: Come up with a name indicating an error if nothing in the polymorph
}



int polymorph_ptr_eq(polymorph_ptr a, polymorph_ptr b) {
    //Equal iff pointers equal
    if (a == b) {
        return 1;
    }
    return 0; //nope! not equal
}


