#include "stdio.h"
#include "../libs/digraph.h"

DEFINE_GRAPH(int)

noderef printRef(noderef in) {
    printf("node %d \n", in);
    return in;
}

main() {
    noderef nodelocs[5];
    int_graph test_graph = int_graph_init(3);
    test_graph = int_graph_addnode(test_graph, 0, &nodelocs[0]);
    test_graph = int_graph_addnode(test_graph, 1, &nodelocs[1]);
    test_graph = int_graph_addnode(test_graph, 2, &nodelocs[2]);
    test_graph = int_graph_addnode(test_graph, 3, &nodelocs[3]);
    test_graph = int_graph_addnode(test_graph, 4, &nodelocs[4]);
    test_graph = int_graph_addedge(test_graph, nodelocs[0], nodelocs[1]);
    test_graph = int_graph_addedge(test_graph, nodelocs[1], nodelocs[3]);
    test_graph = int_graph_addedge(test_graph, nodelocs[2], nodelocs[1]);
    test_graph = int_graph_addedge(test_graph, nodelocs[2], nodelocs[3]);
    test_graph = int_graph_addedge(test_graph, nodelocs[3], nodelocs[4]);
    flag_mat_print(test_graph.adjmat); 
    noderef_dynarray_free(noderef_dynarray_map(int_graph_get_roots(test_graph), printRef));
    printf(" \n");
    noderef_dynarray_free(noderef_dynarray_map(int_graph_topo_sort(test_graph), printRef));
    return 0;
}
