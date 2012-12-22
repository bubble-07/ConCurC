#include "dynarray.h"
#ifndef SLICE_DEFINED
#define SLICE_DEFINED
typedef struct {
    size_t size;
    int *  begin;
} int_slice;

int_slice int_slice_init(int* ptr, size_t dim) {
    int_slice result;
    result.begin = ptr;
    result.size = dim;
}

/*Indexes are inclusive. NOTE: It is ABSOLUTELY UNSAFE to have slices for a dynarray that
mutates! TODO: Throw errors on bad input!*/
int_slice int_slice_fromarray(int* ptr, size_t lowindex, size_t highindex) {
    int_slice result;
    result.begin = ptr + lowindex;
    result.size = highindex - lowindex + 1;
    return result;
}

int int_slice_get(int_slice in, size_t i) {
    return in.begin[i];
}
/*
type##_slice type##_slice_getsubslice(type##_slice in, size_t lowindex, size_t highindex) { \
    return type##_slice_fromarray(in.begin, lowindex, highindex); \
} \
type##_slice type##_slice_fromdynarray(type##_dynarray in) { \
    return type##_slice_fromarray(in.begin, 0, in.size - 1); \
}
*/


#endif
