#include "stdio.h"
#include "../libs/digraph.h"

DEFINE_GRAPH(int)

noderef printRef(noderef in) {
    printf("node %d \n", in);
    return in;
}
void intdestruct(int in) {
    return;
}

main() {
    size_t size = 10;
    noderef current;
    int_graph test_graph = int_graph_init(size);
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
    test_graph = int_graph_addedge(test_graph, 2, 3);
    test_graph = int_graph_addedge(test_graph, 3, 4);
    test_graph = int_graph_addedge(test_graph, 4, 6);

    //test_graph = int_graph_switch(test_graph, 0, 1);
    test_graph = int_graph_gc(test_graph, &intdestruct);
    flag_mat_print(test_graph.adjmat); 
    noderef_dynarray_free(noderef_dynarray_map(int_graph_get_roots(test_graph), printRef));
    printf(" \n");
    noderef_dynarray_free(noderef_dynarray_map(int_graph_topo_sort(test_graph), printRef));
    //int_graph_topo_sort(test_graph);
    return 0;
}
