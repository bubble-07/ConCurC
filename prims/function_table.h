//Implements a dictionary from strings to polymorphs and supporting methods
#include "../libs/dict.h"
#include "../libs/dynstring.h"
#include "polymorph.h"

#ifndef FUNCTIONTABLE_DEFINED
#define FUNCTIONTABLE_DEFINED

DEFINE_DICT(string, polymorph)
//Define something that lets us get all stored polymorphs
DEFINE_GET_ALL_VALS(string, polymorph)


typedef struct {
    string_polymorph_dict dict;
} function_table;

static function_table init_function_table() {
    function_table result;
    result.dict = string_polymorph_dict_init(2);
    return result;
}
//Add an empty polymorph with the given name
//If it already exists, do nothing
static function_table add_empty_polymorph(function_table in, string s) {
    in.dict = string_polymorph_dict_addNoDup(in.dict,
                                    string_polymorph_bucket_make(s, make_empty_polymorph()));
    return in;
}

//Returns a dynamic array of all stored polymorphs
static polymorph_dynarray get_all_polymorphs(function_table in) {
    return string_polymorph_dict_get_all_values(in.dict);
}

//Returns 1 if a given function name has already been registered
static int has_polymorph(function_table in, string s) {
    return string_polymorph_dict_exists(in.dict, s);
}


#endif

