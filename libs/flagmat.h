#include <stdlib.h>
#include <stdio.h>
#include "memoryman.h"
#include "dynarray.h"

#ifndef FLAGMAT_DEFINED
#define FLAGMAT_DEFINED

typedef char flag;
DEFINE_DYNARRAY(flag)
DEFINE_DYNARRAY(flag_dynarray)

typedef flag_dynarray_dynarray flag_mat;

#define FLAG_TRUE 1
#define FLAG_FALSE 0

static inline flag_mat flag_mat_init(size_t rows, size_t columns) {
    flag_mat result = flag_dynarray_dynarray_make(rows + 2);
    int i;
    for (i=0; i < rows; i++) {
        result = flag_dynarray_dynarray_add(result, flag_dynarray_make(columns + 2));
    }
    return result;
}
static inline flag_mat flag_mat_addzeroedcolumn(flag_mat in) {
    int i;
    for (i=0; i < in.size; i++) {
        in.begin[i] = flag_dynarray_add(in.begin[i], FLAG_FALSE);
    }
    return in;
}
static inline flag_dynarray flag_dynarray_zeroedrow(size_t s) {
    flag_dynarray result = flag_dynarray_make(s + 2);
    size_t i;
    for (i=0; i < s; i++) {
        result = flag_dynarray_add(result, FLAG_FALSE);
    }
    return result;
}

static inline void flag_mat_free(flag_mat in) {
    size_t i;
    for (i=0; i < in.size; i++) {
        flag_dynarray_free(in.begin[i]);
    }
    flag_dynarray_dynarray_free(in);
    return;
}

static inline void flag_mat_print(flag_mat in) {
    int i;
    int j;
    printf("%s", "\n");
    for (i=0; i < in.size; i++) {
        for (j=0; j < in.begin[i].size; j++) {
            if (in.begin[i].begin[j] == FLAG_TRUE) {
                printf("%d", 1);
            }
            else {
                printf("%d", 0);
            }
        }
        printf("%s", "\n");
    }
    return;
}
static inline flag_mat flag_mat_copy(flag_mat in) {
    flag_mat result = flag_mat_init(in.size, in.size);
    size_t i;
    for (i=0; i < in.size; i++) {
        result.begin[i] = flag_dynarray_copy(in.begin[i]);
    }
    return result;
}
static inline flag_mat flag_mat_swaprows(flag_mat in, size_t one, size_t two) {
    flag_dynarray tmp = in.begin[one];
    in.begin[one] = in.begin[two];
    in.begin[two] = tmp;
    return in;
}
static inline flag_mat flag_mat_swapflags(flag_mat in, size_t rowone, size_t colone,
                                                       size_t rowtwo, size_t coltwo) {
    flag tmp = in.begin[rowone].begin[colone];
    in.begin[rowone].begin[colone] = in.begin[rowtwo].begin[coltwo];
    in.begin[rowtwo].begin[coltwo] = tmp;
    return in;
}
static inline flag_mat flag_mat_swapcols(flag_mat in, size_t one, size_t two) {
    size_t i;
    for (i=0; i < in.size; i++) {
        in = flag_mat_swapflags(in, i, one, i, two);
    }
    return in;
}
static inline flag_mat flag_mat_swapboth(flag_mat in, size_t one, size_t two) {
    in = flag_mat_swaprows(in, one, two);
    in = flag_mat_swapcols(in, one, two);
    return in;
}
    

#endif 
