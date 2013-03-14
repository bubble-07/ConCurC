/* Implementation of an unweighted digraph -- n^2 transitive closure, etc */
#include <stdlib.h>
#include "memoryman.h"
#include "dynarray.h"

typedef char flag;
DEFINE_DYNARRAY(flag)
DEFINE_DYNARRAY(flag_dynarray)

typedef flag_dynarray_dynarray flag_mat;

#define FLAG_TRUE 1
#define FLAG_FALSE 0

flag_mat flag_mat_init(size_t rows, size_t columns) {
    flag_mat result = flag_dynarray_dynarray_make(rows + 2);
    int i;
    for (i=0; i < rows; i++) {
        result = flag_dynarray_dynarray_add(result, flag_dynarray_make(columns + 2));
    }
    return result;
}
flag_mat flag_mat_addzeroedcolumn(flag_mat in) {
    int i;
    for (i=0; i < in.size; i++) {
        in.begin[i] = flag_dynarray_add(in.begin[i], FLAG_FALSE);
    }
    return in;
}
flag_dynarray flag_dynarray_zeroedrow(size_t s) {
    flag_dynarray result = flag_dynarray_make(s + 2);
    size_t i;
    for (i=0; i < s; i++) {
        result = flag_dynarray_add(result, FLAG_FALSE);
    }
    return result;
}

void flag_mat_free(flag_mat in) {
    size_t i;
    for (i=0; i < in.size; i++) {
        flag_dynarray_free(in.begin[i]);
    }
    flag_dynarray_dynarray_free(in);
    return;
}

void flag_mat_print(flag_mat in) {
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
flag_mat flag_mat_copy(flag_mat in) {
    flag_mat result = flag_mat_init(in.size, in.size);
    size_t i;
    for (i=0; i < in.size; i++) {
        result.begin[i] = flag_dynarray_copy(in.begin[i]);
    }
    return result;
}
    

DEFINE_DYNARRAY(int)

typedef struct {
    flag_mat adjmat;
    int_dynarray nodes;
    size_t size;
    size_t rsize;
} int_digraph;

typedef size_t noderef;

DEFINE_DYNARRAY(noderef);

int_digraph int_digraph_init(size_t expectednumnodes) {
    int_digraph result;
    result.adjmat = flag_mat_init(expectednumnodes, expectednumnodes);
    result.nodes = int_dynarray_make(expectednumnodes + 2);
    result.size = 0;
    result.rsize = expectednumnodes;
    return result;
}
int_digraph int_digraph_addnode(int_digraph in, int toadd) {
    in.nodes = int_dynarray_add(in.nodes, toadd);
    if (!(in.size < in.rsize)) {
        in.adjmat = flag_dynarray_dynarray_add(in.adjmat, flag_dynarray_zeroedrow(in.size));
        in.rsize += 1;
    }
    in.adjmat = flag_mat_addzeroedcolumn(in.adjmat);
    in.size += 1;
    return in;
}
int_digraph int_digraph_addedge(int_digraph in, noderef n1, noderef n2) {
    in.adjmat.begin[n1].begin[n2] = FLAG_TRUE;
    return in;
}
int int_digraph_testedge(int_digraph in, noderef n1, noderef n2) {
    return in.adjmat.begin[n1].begin[n2];
}
noderef_dynarray int_digraph_getchildren(int_digraph in, noderef current) {
    noderef_dynarray result = noderef_dynarray_make(in.size);
    noderef i;
    for (i=0; i < in.size; i++) {
        if (in.adjmat.begin[current].begin[i] == FLAG_TRUE) {
            result = noderef_dynarray_add(result, i);
        }
    }
    return result;
}
int_digraph int_digraph_copy(int_digraph in) {
    int_digraph result = in;
    result.adjmat = flag_mat_copy(in.adjmat);
    result.nodes = int_dynarray_copy(in.nodes);
    return result;
}

int_digraph int_digraph_floydwarshall(int_digraph in) {
    int i;
    int j;
    int k;
    for (k=0; k < in.size; k++) {
        for (i=0; i < in.size; i++) {
            for (j=0; j < in.size; j++) {
                /*
                if (int_digraph_testedge(in, i, k) & int_digraph_testedge(in, k, j)) {
                    in = int_digraph_addedge(in, i, j);
                } */
                in.adjmat.begin[i].begin[j] = 
                    in.adjmat.begin[i].begin[k] & in.adjmat.begin[k].begin[j] | 
                    in.adjmat.begin[i].begin[j];
            }
        }
    }
    return in;
}
int_digraph int_digraph_transitiveclosure(int_digraph in) {
    in = int_digraph_floydwarshall(in);
    return in;
}


int_digraph int_digraph_free(int_digraph in) {
    int_dynarray_free(in.nodes);
    flag_mat_free(in.adjmat);
    return;
}

int main() {
    int_digraph test = int_digraph_init(1);
    size_t i;
    size_t s = 16384;
    for (i=0; i < s; i++) {
        test = int_digraph_addnode(test, i);
    }
    for (i=0; i < s-2; i++) {
        test = int_digraph_addedge(test, i, i + 1);
        test = int_digraph_addedge(test, i, i + 2);
    }
    //flag_mat_print(test.adjmat);
    test = int_digraph_transitiveclosure(test);
    //printf("%s", "COMPLETE! \n");
    //flag_mat_print(test.adjmat);
    int_digraph_free(test);
    return 0;
}
