#ifndef DYNARRAY_DEFINED
#define DYNARRAY_DEFINED
#define DEFINE_DYNARRAY_TYPE(type) \
typedef struct type##_dynarray type##_dynarray; \
struct type##_dynarray { \
    int rsize; \
    int size; \
    type * begin; \
}; 

 #define DEFINE_DYNARRAY_METHODS(type) \
type##_dynarray type##_dynarray_make(int startsize) { \
    type##_dynarray result; \
    result.size = 0; \
    result.rsize = startsize; \
    result.begin = memalloc(startsize * sizeof(type)); \
    return result; \
} \
 \
type##_dynarray type##_dynarray_expand(type##_dynarray in) { \
    type##_dynarray result = type##_dynarray_make(in.rsize * 2); \
    result.size = in.size; \
    memcpy( result.begin,(const void*) in.begin, in.rsize * sizeof(type)); \
    free(in.begin); \
    return result; \
} \
 \
type##_dynarray type##_dynarray_checkexp(type##_dynarray in) { \
    if (in.size + 1 > in.rsize) { \
        return type##_dynarray_expand(in); \
    } \
    return in; \
} \
     \
 \
type##_dynarray type##_dynarray_add(type##_dynarray in, type val) { \
    in = type##_dynarray_checkexp(in); \
    in.begin[in.size] = val; \
    in.size++; \
    return in; \
} \
 \
type##_dynarray type##_dynarray_cat(type##_dynarray one, type##_dynarray two) { \
    int i = 0; \
    while (i < two.size) { \
        one = type##_dynarray_add(one, two.begin[i]); \
        i++; \
    } \
    return one; \
} \
type##_dynarray type##_dynarray_copy(type##_dynarray in) { \
    type##_dynarray result = type##_dynarray_make(1); \
    result = type##_dynarray_cat(result, in); \
    return result; \
} \
void type##_dynarray_free(type##_dynarray in) { \
    free(in.begin); \
} \
 \
type##_dynarray type##_dynarray_fromarray(type * values, int len) { \
    int x = 0; \
    type##_dynarray result = type##_dynarray_make(1);  \
    while (x < len) { \
        result = type##_dynarray_add(result, values[x]); \
        x++; \
    } \
    return result; \
} \
 \
type##_dynarray type##_dynarray_map(type##_dynarray in, type (*f)(type)) { \
    int x = 0; \
    while (x < in.size) { \
        in.begin[x] = (*f)(in.begin[x]); \
        x++; \
    } \
    return in; \
} \
type type##_dynarray_foldl(type##_dynarray in, type (*f)(type, type), type start) { \
    size_t i = 0; \
    while (i < in.size) { \
        start = f(start, in.begin[i]); \
        i++; \
    } \
    return start; \
} 
#define DEFINE_DYNARRAY(type) \
DEFINE_DYNARRAY_TYPE(type) \
DEFINE_DYNARRAY_METHODS(type)
#endif
