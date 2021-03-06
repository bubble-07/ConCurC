//Defines type_envs, which are used to define mappings from lexids posing as type variables
//to type_refs

#include "../libs/dict.h"
#include "type_ref.h"
#include "lexid.h"

#ifndef TYPE_ENV_DEFINED
#define TYPE_ENV_DEFINED

DEFINE_DICT(lexid, type_ref)
DEFINE_GET_ALL_VALS(lexid, type_ref)

typedef lexid_type_ref_dict type_env;

type_env type_env_init();
type_env type_env_add(type_env in, lexid name, type_ref binding);
type_ref type_env_lookup(type_env in, lexid name);
int type_env_exists(type_env in, lexid name);
//Given a type_env, returns a list of all type refs that are in it
type_ref_dynarray type_env_extract_type_refs(type_env in);

#endif
