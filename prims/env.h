//Implements environments for the sake of determining
//which parameter a given lexid corresponds to [lexical scoping]

#include "../prims/lexid.h"
#include "parameter.h"
#include "../libs/dict.h"

#ifndef ENV_DEFINED
#define ENV_DEFINED

DEFINE_DICT(lexid, parameter_ptr)

typedef struct env {
    //Lookup table for bindings within the immediately-enclosing scope
    lexid_parameter_ptr_dict binds;
    //Reference to the parent (null for no parent)
    struct env* parent;
} env;

//Creates an environment with the specified parent
static env make_new_env(env* p) {
    env result;
    result.binds = lexid_parameter_ptr_dict_init(5);
    result.parent = p;
    return result;
}
//Frees the data referenced by the immediate scope
//of the passed environment
static void free_env(env in) {
    lexid_parameter_ptr_dict_free(in.binds);
    return;
}

//Adds a single parameter pointer to the environment
//Assumption: Non-null pointer, and name already given in p
static env add_param_to_env(env in, parameter_ptr p) {
    lexid name = p->name;
    in.binds = lexid_parameter_ptr_dict_quickadd(in.binds, name, p);
    return in;
}


//Adds a dynamic array of parameters one-by-one to the environment
//Note: this internally uses parameter_dynarray_getptr, which is UNSAFE
//if the dynamic array of parameters is mutated at any point after calling this
static env add_params_to_env(env in, parameter_dynarray toadd) {
    int i;
    for (i=0; i < toadd.size; i++) {
        //Add a pointer to the current element in the dynarray to the environment
        in = add_param_to_env(in, parameter_dynarray_getptr(toadd, i));
    }
    return in;
}

//Creates a fork of the environment passed with the requested params
static env fork_env(env* in, parameter_dynarray toadd) {
    env result = make_new_env(in);
    result = add_params_to_env(result, toadd);
    return result;
}

//Creates a new environment from a dynamic array of params
static env params_to_env(parameter_dynarray in) {
    return add_params_to_env(make_new_env(NULL), in);
}

//Finds the parameter in the innermost scope corresponding to the environment in
//associated with the lexid name
static parameter_ptr env_lookup(env in, lexid name) {
    //Get the result of looking in the enclosing scope
    parameter_ptr result = lexid_parameter_ptr_dict_get(in.binds, name);
    //If we successfully found the parameter
    if (!parameter_ptr_eq(result, parameter_ptr_lookup_failure)) {
        return result;
    }
    else {
        //Lookup failed. Check to see if we have a parent environment
        if (in.parent != NULL) {
            return env_lookup(*in.parent, name); //Continue the search in the parent
        }
    }
    //Otherwise, the lookup must've failed and we have no parent, so fail
    return result;
}
    
#endif
    
