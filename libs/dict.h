#include "dynarray.h"
#include "memoryman.h"

#ifndef DICT_DEFINED
#define DICT_DEFINED
#define DEFINE_DICT(keyT, valT) \
typedef struct { \
    keyT key; \
    valT value; \
} keyT##_##valT##_bucket; \
 \
inline static keyT##_##valT##_bucket keyT##_##valT##_bucket_make(keyT key, valT value) { \
    keyT##_##valT##_bucket result; \
    result.key = key; \
    result.value = value; \
    return result; \
} \
 \
DEFINE_DYNARRAY(keyT##_##valT##_bucket) \
 \
DEFINE_DYNARRAY(keyT##_##valT##_bucket_dynarray) \
 \
typedef keyT##_##valT##_bucket_dynarray_dynarray keyT##_##valT##_dict; \
 \
inline static keyT##_##valT##_bucket_dynarray keyT##_##valT##_initrow(keyT##_##valT##_bucket_dynarray in) { \
    return keyT##_##valT##_bucket_dynarray_make(1); \
} \
 \
inline static keyT##_##valT##_dict keyT##_##valT##_dict_init(size_t size) { \
    keyT##_##valT##_dict result; \
    result = keyT##_##valT##_bucket_dynarray_dynarray_make(size); \
    result.size = size; \
    result = keyT##_##valT##_bucket_dynarray_dynarray_map(result, & keyT##_##valT##_initrow); \
    return result; \
} \
 \
inline static keyT##_##valT##_dict keyT##_##valT##_dict_add(keyT##_##valT##_dict in, keyT##_##valT##_bucket to_add) { \
     size_t i = hash_##keyT(to_add.key) % in.size; \
     in.begin[i] = keyT##_##valT##_bucket_dynarray_add(in.begin[i], to_add); \
     return in; \
} \
inline static valT keyT##_##valT##_dict_get(keyT##_##valT##_dict in, keyT key) { \
     size_t i = hash_##keyT(key) % in.size; \
     keyT##_##valT##_bucket_dynarray row = in.begin[i]; \
     i = 0; \
     while (i < row.size) { \
         if (keyT##_eq(row.begin[i].key, key)) { \
             return row.begin[i].value; \
         } \
         i++; \
     } \
     valT result = valT##_lookup_failure; \
     return result; \
} \
inline static keyT##_##valT##_dict keyT##_##valT##_dict_addNoDup(keyT##_##valT##_dict in, keyT##_##valT##_bucket to_add) { \
    valT lookupval = keyT##_##valT##_dict_get(in, to_add.key); \
    if (! valT##_eq(lookupval, valT##_lookup_failure)) { \
        return in; \
    } \
    in = keyT##_##valT##_dict_add(in, to_add); \
    return in; \
}
#endif
