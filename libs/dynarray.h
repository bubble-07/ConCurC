#include <stdlib.h>
#include "memoryman.h"

#ifndef DYNARRAY_DEFINED
#define DYNARRAY_DEFINED

/*Implementation of type-generic macro-defined dynamic arrays, of the form type##_dynarray, using
a dynamically allocated, automatically doubling array. */


/*O(1) Lookup, O(n) resize, O(1) amortized append */

/*Declares the type of the dynarray*/
#define DEFINE_DYNARRAY_TYPE(type) \
typedef struct type##_dynarray type##_dynarray; \
struct type##_dynarray { \
    int rsize; \
    int size; \
    type * begin; \
}; \
static type##_dynarray empty_##type##_dynarray = {0, 0, NULL}; \
static type##_dynarray type##_dynarray_lookup_failure = {0, 0, NULL};

/*Declares the methods of a dynarray*/
 #define DEFINE_DYNARRAY_METHODS(type) \
 /*Creates a dynarray with a given initial virtual size*/ \
inline static type##_dynarray type##_dynarray_make(int startsize) { \
    type##_dynarray result; \
    result.size = 0; \
    result.rsize = startsize; \
    result.begin = memalloc(startsize * sizeof(type)); \
    return result; \
} \
 /* Expands the dynarray's virtual size by a factor of two*/\
inline static type##_dynarray type##_dynarray_expand(type##_dynarray in) { \
    type##_dynarray result = type##_dynarray_make(in.rsize * 2); \
    result.size = in.size; \
    memcpy( result.begin,(const void*) in.begin, in.rsize * sizeof(type)); \
    free(in.begin); \
    return result; \
} \
 /*Checks if the dynarray's size is approaching its virtual size, and resizes if needed */\
inline static type##_dynarray type##_dynarray_checkexp(type##_dynarray in) { \
    if (in.size + 1 > in.rsize) { \
        return type##_dynarray_expand(in); \
    } \
    return in; \
} \
     \
/*Adds an element to the end of a dynarray*/ \
inline static type##_dynarray type##_dynarray_add(type##_dynarray in, type val) { \
    in = type##_dynarray_checkexp(in); \
    in.begin[in.size] = val; \
    in.size++; \
    return in; \
} \
/* Gets an element of a dynarray TODO: maybe add error handling and use this more often*/ \
inline static type type##_dynarray_get(type##_dynarray in, int i) { \
    return in.begin[i]; \
}\
/*Gets a pointer to an element of a dynarray. UNSAFE if the dynarray is modified while \
the pointer is still kept, so ensure you're done mutating it when you use this*/ \
inline static type * type##_dynarray_getptr(type##_dynarray in, int i) { \
    return &in.begin[i]; \
} \
/*concatenates the second dynarray onto the first*/ \
inline static type##_dynarray type##_dynarray_cat(type##_dynarray one, type##_dynarray two) { \
    int i = 0; \
    while (i < two.size) { \
        one = type##_dynarray_add(one, two.begin[i]); \
        i++; \
    } \
    return one; \
} \
/*creates a deep copy of a dynarray, and returns it*/ \
inline static type##_dynarray type##_dynarray_copy(type##_dynarray in) { \
    type##_dynarray result = type##_dynarray_make(1); \
    result = type##_dynarray_cat(result, in); \
    return result; \
} \
/*frees a dynarray's data */ \
inline static void type##_dynarray_free(type##_dynarray in) { \
    free(in.begin); \
} \
 /*moves an element in a dynarray to the beginning, and moves all other elements over to 
 accomodate this change */\
inline static type##_dynarray type##_dynarray_moveToBegin(size_t i, type##_dynarray in) { \
    type tmp = in.begin[0]; \
    type tmp2; \
    in.begin[0] = in.begin[i]; \
    size_t iter = 1; \
    while (iter < i + 1) { \
        tmp2 = in.begin[iter]; \
        in.begin[iter] = tmp; \
        iter++; \
        tmp = tmp2; \
    } \
    return in; \
} \
/*Sorts a given dynarray according to the passed comparison function. This method
internally uses quicksort with qsort, which explains the funky type signature. The comparison
function should return -1 if the first elem is before the second, 0 if equivalent, 1 else.*/ \
inline static type##_dynarray type##_dynarray_sort(type##_dynarray in, int (*compare)(const void*, const void*)) { \
    qsort(in.begin, in.size, sizeof(type), compare); \
    return in; \
} \
/*creates a dynarray from an array and a given number of elements to read */\
inline static type##_dynarray type##_dynarray_fromarray(type * values, int len) { \
    int x = 0; \
    type##_dynarray result = type##_dynarray_make(1);  \
    while (x < len) { \
        result = type##_dynarray_add(result, values[x]); \
        x++; \
    } \
    return result; \
} \
 /*implements the higher-order map function on a dynarray*/\
inline static type##_dynarray type##_dynarray_map(type##_dynarray in, type (*f)(type)) { \
    int x = 0; \
    while (x < in.size) { \
        in.begin[x] = (*f)(in.begin[x]); \
        x++; \
    } \
    return in; \
} \
/* append "len" number of "val" to "in" */ \
inline static type##_dynarray type##_dynarray_fill(type##_dynarray in, type val, size_t len) { \
    size_t i; \
    for (i = 0; i < len; i++) { \
        in = type##_dynarray_add(in, val); \
    } \
    return in; \
} \
\
/*Left-folds on dynarrays. Takes a dynarray, a function to fold over it, and the "starting" or \
"zero" element. Returns the value obtained by carrying out a higher-order left fold*/\
inline static type type##_dynarray_foldl(type##_dynarray in, type (*f)(type, type), type start) { \
    size_t i = 0; \
    while (i < in.size) { \
        start = f(start, in.begin[i]); \
        i++; \
    } \
    return start; \
} \
/*Gets the subsequence of the dynarray */ \
inline static type##_dynarray type##_dynarray_get_subsequence(type##_dynarray in,  \
                                                              size_t lbound, size_t rbound) { \
    type##_dynarray result = type##_dynarray_make((rbound - lbound) + 1); \
    for (lbound = lbound; lbound < (rbound + 1); lbound++) { \
        result = type##_dynarray_add(result, in.begin[lbound]); \
    } \
    return result; \
} \
/*Takes a dynarray and a predicate function, and returns a new dynarray of \
elements that satisfy the predicate. */ \
inline static type##_dynarray type##_dynarray_filter(type##_dynarray in, int(*P)(type)) { \
    type##_dynarray result = type##_dynarray_make(1); \
    size_t i; \
    for (i=0; i < in.size; i++) { \
        if (P(in.begin[i])) { \
            result = type##_dynarray_add(result, in.begin[i]); \
        } \
    } \
    return result; \
} \
/* Same thing, but the original dynarray is replaced, and all references to the original will go bad */ \
inline static type##_dynarray type##_dynarray_filter_inplace(type##_dynarray in, int(*P)(type)) { \
    type##_dynarray tmp = type##_dynarray_filter(in, P); \
    type##_dynarray_free(in); \
    return tmp; \
} 

/*Convenient macro to define both the type and methods of a given dynarray */
#define DEFINE_DYNARRAY(type) \
DEFINE_DYNARRAY_TYPE(type) \
DEFINE_DYNARRAY_METHODS(type)
#endif
