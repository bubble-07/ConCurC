#include "stdio.h"
#include "../libs/digraph.h"

DEFINE_GRAPH(int)

int_graph test_graph;

noderef printRef(noderef in) {
    printf("node %d \n", in);
    printf("number %d \n", test_graph.nodes.begin[in]);
    return in;
}
void intdestruct(int in) {
    return;
}
int mergeInts(int_dynarray in) {
    size_t i;
    int result = 0;
    for (i=0; i < in.size; i++) {
        result += in.begin[i];
    }
    return result;
}

DEFINE_MERGE_CYCLES(int)

main() {
    size_t size = 4;
    noderef current;
    test_graph = int_graph_init(size);
    size_t i;
    
    for (i=0; i < size; i++) {
        test_graph = int_graph_addnode(test_graph, i, &current);
    }
    /*
    for (i=0; i < (size - 6); i = i + 2) {
        test_graph = int_graph_addedge(test_graph, i, i + 2);
        test_graph = int_graph_addedge(test_graph, i, i + 4);
        test_graph = int_graph_addedge(test_graph, i, i + 6);
    } */
    test_graph = int_graph_addedge(test_graph, 1, 2);
    test_graph = int_graph_addedge(test_graph, 2, 3);
    test_graph = int_graph_addedge(test_graph, 3, 1);

    //test_graph = int_graph_switch(test_graph, 0, 1);
    //test_graph = int_graph_gc(test_graph, &intdestruct);
    noderef_dynarray collapsees = noderef_dynarray_make(1);
    collapsees = noderef_dynarray_add(collapsees, 1);
    collapsees = noderef_dynarray_add(collapsees, 2);
    collapsees = noderef_dynarray_add(collapsees, 3);
    test_graph = int_graph_collapse(test_graph, &mergeInts, collapsees);
    test_graph = int_graph_gc(test_graph, &intdestruct);
    flag_mat_print(test_graph.adjmat); 
    noderef_dynarray_free(noderef_dynarray_map(int_graph_get_roots(test_graph), printRef));
    printf(" \n");
    printf("number: %i", test_graph.nodes.begin[0]);
    noderef_dynarray_free(noderef_dynarray_map(int_graph_topo_sort(test_graph), printRef));
    //int_graph_topo_sort(test_graph);
    return 0;
}
