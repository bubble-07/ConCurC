/* Implementation of an unweighted digraph -- n^2 transitive closure, etc */
#include <stdlib.h>
#include "memoryman.h"
#include "dynarray.h"

typedef int flag;
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

int_digraph int_digraph_gbanclosure(int_digraph in, noderef current, noderef_dynarray crumbs) {
    size_t i;
    for (i=0; i < crumbs.size; i++) {
        in = int_digraph_addedge(in, crumbs.begin[i], current);
    }
    noderef_dynarray children = int_digraph_getchildren(in, current);
    noderef_dynarray tmp;
    for (i=0; i < children.size; i++) {
        tmp = noderef_dynarray_add(noderef_dynarray_copy(crumbs), current);
        in = int_digraph_gbanclosure(in, children.begin[i], tmp);
    }
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
    size_t s = 8000;
    for (i=0; i < s; i++) {
        test = int_digraph_addnode(test, i);
    }
    for (i=0; i < s-1; i++) {
        test = int_digraph_addedge(test, i, i + 1);
    }
    test = int_digraph_gbanclosure(test, 0, noderef_dynarray_make(1));
    int_digraph_free(test);
    //flag_mat_print(test.adjmat);
    return 0;
}
