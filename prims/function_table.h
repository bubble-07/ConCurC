//Implements a dictionary from lexids to polymorphs and supporting methods

#include "../libs/dict.h"
#include "../libs/dynstring.h"
#include "polymorph.h"
#include "../libs/memoryman.h"
#include "lexid.h"

#ifndef FUNCTIONTABLE_DEFINED
#define FUNCTIONTABLE_DEFINED

DEFINE_DICT(lexid, polymorph)
//Define something that lets us get all stored polymorphs
DEFINE_GET_ALL_VALS(lexid, polymorph)
DEFINE_GET_PTR(lexid, polymorph)


typedef struct {
    lexid_polymorph_dict dict;
} function_table;

static function_table init_function_table() {
    function_table result;
    result.dict = lexid_polymorph_dict_init(50);
    return result;
}
//Add an empty polymorph with the given name
//If it already exists, do nothing
static function_table add_empty_polymorph(function_table in, lexid s) {
    in.dict = lexid_polymorph_dict_addNoDup(in.dict,
                                    lexid_polymorph_bucket_make(s, make_empty_polymorph()));
    return in;
}

//Returns a dynamic array of all stored polymorphs
static polymorph_dynarray get_all_polymorphs(function_table in) {
    return lexid_polymorph_dict_get_all_values(in.dict);
}

//Returns 1 if a given function name has already been registered
static int has_polymorph(function_table in, lexid s) {
    return lexid_polymorph_dict_exists(in.dict, s);
}

//Adds a function pointer to the polymorph with the given name (assumes it exists)
static function_table add_function_ptr(function_table in, lexid name, function_ptr f) {
    //Get the polymorph corresponding to the name
    polymorph* target = lexid_polymorph_dict_getptr(in.dict, name);
    //Add the given function to the polymorph (which is still within the dict)
    *target = add_to_polymorph(*target, f);
    //Return the function table we modified.
    return in;

}

//Same as above, but automatically copies the function f into new memory and calls
//the above method on a pointer to this new copy
static function_table add_function(function_table in, lexid name, function f) {
    function_ptr func = (function*) memalloc(sizeof(function));
    *func = f;
    return add_function_ptr(in, name, func);
}


//Gets the polymorph corresponding to a given name
static polymorph get_polymorph(function_table in, lexid name) {
    return lexid_polymorph_dict_get(in.dict, name);
}

//Gets us a pointer to the polymorph with the given name (assumes it exists)
//NOTE: this is inherently UNSAFE if the pointed-to polymorph moves.
static polymorph_ptr get_polymorph_ptr(function_table in, lexid name) {
    return lexid_polymorph_dict_getptr(in.dict, name);
}

   


#endif

