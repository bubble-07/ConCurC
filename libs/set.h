#include "dict.h"

#define DEFINE_SET(type) \
typedef int setflag; \
setflag setflag_lookup_failure = 0; \
int setflag_eq(setflag one, setflag two) { \
    return one == two; \
} \
DEFINE_DICT(type, setflag) \
typedef type##_setflag_dict type##_set; \
\
inline static type##_set type##_set_init(size_t size) { \
    return type##_setflag_dict_init(size); \
} \
inline static type##_set type##_set_add(type##_set set, type elem) { \
    return type##_setflag_dict_addNoDup(set, type##_setflag_bucket_make(elem, 1)); \
} \
inline static int type##_set_isElem(type##_set set, type elem) { \
    setflag test = type##_setflag_dict_get(set, elem); \
    if (setflag_eq(test, setflag_lookup_failure)) { \
        return 0; \
    } \
    return 1; \
} \
inline static void type##_set_free(type##_set set) { \
    type##_setflag_dict_free(set); \
    return; \
}

