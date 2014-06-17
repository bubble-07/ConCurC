
#ifndef POLYMORPH_DEFINED
#define POLYMORPH_DEFINED

#include "function.h"

struct polymorph {
    function_ptr_dynarray options; //List of functions that may work
    int local; //Should be set to 1 if the polymorph in question is local (not in a function table) else, 0
};
typedef struct polymorph polymorph;

typedef polymorph* polymorph_ptr;

DEFINE_DYNARRAY(polymorph)
DEFINE_DYNARRAY(polymorph_ptr)

//Creates a non-local polymorph with no function options
static polymorph make_empty_polymorph() {
    polymorph result;
    result.options = function_ptr_dynarray_make(1);
    result.local = 0;
    return result;
}

static int polymorph_ptr_numoptions(polymorph_ptr in) {
    return in->options.size;
}

static function_ptr_dynarray polymorph_ptr_get_options(polymorph_ptr in) {
    return in->options;
}

//Gets a sum type of all possible types the parameter in position pos could be
static type_ref polymorph_ptr_get_parameter_type(polymorph_ptr in, int pos) {
    function_ptr_dynarray options = polymorph_ptr_get_options(in);
    type_ref result = make_empty_type_ref();
    int i;
    for (i=0; i < options.size; i++) {
        //Add more options for what the type can be
        result = concat_type_refs(result, function_ptr_get_parameter_type(options.begin[i], pos));
    }
    return result;
}

static type_ref polymorph_ptr_get_return_type(polymorph_ptr in) {
    polymorph_ptr_dynarray options = polymorph_ptr_get_options(in);
    type_ref result = make_empty_type_ref();
    int i;
    for (i=0; i < options.size; i++) {
        result = concat_type_refs(result, function_ptr_get_return_type(options.begin[i]));
    }
    return result;
}

//Same as make_empty_polymorph, but allocates on heap and returns a pointer
static polymorph_ptr make_empty_polymorph_ptr() {
    polymorph_ptr result = memalloc(sizeof(polymorph));
    *result = make_empty_polymorph();
    return result;
}

//Makes a local copy of the referenced polymorph
static polymorph_ptr polymorph_ptr_copy(polymorph_ptr in) {
    polymorph_ptr result = memalloc(sizeof(polymorph));
    *result = *in;
    result->local = 1;
    return result;
}


//adds the given function pointer to the polymorph
static polymorph add_to_polymorph(polymorph in, function_ptr f) {
    //TODO: Update the most general type returned when you do this!
    in.options = function_ptr_dynarray_add(in.options, f);
    return in;
}

static void print_polymorph_ptr(polymorph_ptr in, nametable names) {
    int i;
    for (i=0; i < in->options.size; i++) {
        print_function(*(in->options.begin[i]), names);
        printf("\n");
    }
    return;
}

//Prints the name of the referred-to polymorph
static void print_polymorph_ptr_name(polymorph_ptr in, nametable names) {
    if (in->options.size > 0) {
        print_function_ptr_name(in->options.begin[0], names);
    }
    return; //TODO: Come up with a name indicating an error if nothing in the polymorph
}


static polymorph_ptr polymorph_ptr_lookup_failure = NULL;

static int polymorph_ptr_eq(polymorph_ptr a, polymorph_ptr b) {
    //Equal iff pointers equal
    if (a == b) {
        return 1;
    }
    return 0; //nope! not equal
}


#endif
