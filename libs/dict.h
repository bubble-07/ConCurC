#include "dynarray.h"
#include "memoryman.h"

/* This header implements type-specified macros to implement generic dictionaries. 
Internally, the dictionaries are represented by generic dynamic arrays of bucket dynamic arrays,
with a user-defined hash function [hash_##keyT] {externally declared}. A lookup is accomplished
by looking up the "row" that a bucket appears in using the hash function, and then traversing
the "row" to find the desired bucket. In order to accomplish this, it is necessary for the user
to define keyT##_eq to test key equivalence before being able to define a dictionary. 
In addition, it is also necessary for the user to define valT##_eq [for addNoDup], 
and valT##_lookup_failure [for a default value a lookup failure]*/

/*O(1) add, O(1) row retrieval, O(n) lookup [although rarely an issue in practice],
resizing is not implemented -- choose your initial size carefully */

#ifndef DICT_DEFINED
#define DICT_DEFINED
#define DEFINE_DICT(keyT, valT) \
/*defines a simple generic bucket type*/ \
typedef struct { \
    keyT key; \
    valT value; \
} keyT##_##valT##_bucket; \
 \
 /*convenience function to create a new bucket object with a given key and value*/ \
inline static keyT##_##valT##_bucket keyT##_##valT##_bucket_make(keyT key, valT value) { \
    keyT##_##valT##_bucket result; \
    result.key = key; \
    result.value = value; \
    return result; \
} \
 /* It is necessary to define dynarrays for the actual dictionary*/\
DEFINE_DYNARRAY(keyT##_##valT##_bucket) \
 \
DEFINE_DYNARRAY(keyT##_##valT##_bucket_dynarray) \
 \
/*a typedef that essentially abstracts a bucket_dynarray_dynarray suffix into a dict suffix */\
typedef keyT##_##valT##_bucket_dynarray_dynarray keyT##_##valT##_dict; \
 \
/*Creates a row. why does it take a dummy parameter? -- see the use of map in dict_init */\
inline static keyT##_##valT##_bucket_dynarray keyT##_##valT##_initrow(keyT##_##valT##_bucket_dynarray in) { \
    return keyT##_##valT##_bucket_dynarray_make(1); \
}\
inline static keyT##_##valT##_bucket_dynarray keyT##_##valT##_freerow(keyT##_##valT##_bucket_dynarray in) { \
    keyT##_##valT##_bucket_dynarray result; \
    keyT##_##valT##_bucket_dynarray_free(in); \
    return result; \
} \
/* initialilzes a dictionary with the given table size. Recommended to be appropriately large*/\
inline static keyT##_##valT##_dict keyT##_##valT##_dict_init(size_t size) { \
    keyT##_##valT##_dict result; \
    result = keyT##_##valT##_bucket_dynarray_dynarray_make(size); \
    result.size = size; \
    result = keyT##_##valT##_bucket_dynarray_dynarray_map(result, & keyT##_##valT##_initrow); \
    return result; \
} \
inline static void keyT##_##valT##_dict_free(keyT##_##valT##_dict in) { \
    in = keyT##_##valT##_bucket_dynarray_dynarray_map(in, & keyT##_##valT##_freerow); \
    keyT##_##valT##_bucket_dynarray_dynarray_free(in); \
    return; \
} \
\
/* adds the bucket given as the second argument to the given dictionary*/ \
inline static keyT##_##valT##_dict keyT##_##valT##_dict_add(keyT##_##valT##_dict in, keyT##_##valT##_bucket to_add) { \
     size_t i = hash_##keyT(to_add.key) % in.size; \
     in.begin[i] = keyT##_##valT##_bucket_dynarray_add(in.begin[i], to_add); \
     return in; \
} \
/* Carries out a lookup operation. If the bucket exists, it returns its value. Otherwise, \
it returns valT##_lookup_failure (user-defined) */ \
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
/* The following method adds a bucket if there isn't already an existing duplicate. To be able \
to use this, valT##_eq needs to be defined as well */ \
inline static keyT##_##valT##_dict keyT##_##valT##_dict_addNoDup(keyT##_##valT##_dict in, keyT##_##valT##_bucket to_add) { \
    valT lookupval = keyT##_##valT##_dict_get(in, to_add.key); \
    if (! valT##_eq(lookupval, valT##_lookup_failure)) { \
        return in; \
    } \
    in = keyT##_##valT##_dict_add(in, to_add); \
    return in; \
}

#define DEFINE_REVERSE_LOOKUP(keyT, valT) \
/* Reverse lookup operation -- O(n), be sure to define valT##_eq and keyT##_lookup_failure*/ \
inline static keyT keyT##_##valT##_dict_reverse_get(keyT##_##valT##_dict in, valT query) { \
    size_t i; \
    size_t j; \
    for (i=0; i < in.size; i++) { \
        for (j=0; j < in.begin[i].size; j++) { \
            if(valT##_eq(in.begin[i].begin[j].value, query)) { \
                return in.begin[i].begin[j].key; \
            } \
        } \
    } \
    return keyT##_lookup_failure; \
}

#endif
