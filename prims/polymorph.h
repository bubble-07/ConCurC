
#ifndef POLYMORPH_DEFINED
#define POLYMORPH_DEFINED

#include "function.h"

struct polymorph {
    function_ptr_dynarray options; //List of functions that may work
    TypeInfo type; //most general type that can be returned
    int isNull; //integer that is set to 1 if the given polymorph is actually null
                //this is used for expressing the results of lookup failures
};
typedef struct polymorph polymorph;

typedef polymorph* polymorph_ptr;

DEFINE_DYNARRAY(polymorph)

static polymorph make_empty_polymorph() {
    polymorph result;
    result.options = function_ptr_dynarray_make(1);
    result.type = make_empty_type();
    result.isNull = 0;
    return result;
}

//adds the given function pointer to the polymorph
static polymorph add_to_polymorph(polymorph in, function_ptr f) {
    //TODO: Update the most general type returned when you do this!
    in.options = function_ptr_dynarray_add(in.options, f);
    return in;
}

static polymorph polymorph_lookup_failure = {{0,0,NULL}, {{0,0,NULL}}, 1};

static int polymorph_eq(polymorph a, polymorph b) {
    if (a.isNull && b.isNull) {
        return 1; //Doing this so we can tell a lookup failure when we see one
    }
    //I can't think of a reason we would test these for equality.
    //However, we will leave this blank so we can define dicts on it
    return 0; //nope! not equal
}


#endif
