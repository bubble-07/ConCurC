#include "type_env.h"

type_env type_env_init() {
    return lexid_type_ref_dict_init(5);
}
type_env type_env_add(type_env in, lexid name, type_ref binding) {
    return lexid_type_ref_dict_quickadd(in, name, binding);
}
type_ref type_env_lookup(type_env in, lexid name) {
    return lexid_type_ref_dict_get(in, name);
}
int type_env_exists(type_env in, lexid name) {
    return lexid_type_ref_dict_exists(in, name);
}
type_ref_dynarray type_env_extract_type_refs(type_env in) {
    return lexid_type_ref_dict_get_all_values(in);
}
