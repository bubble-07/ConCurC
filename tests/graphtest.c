#include <stdlib.h>
#include "../libs/digraph.h"
#include "../libs/dict.h"
#include "../libs/set.h"

int int_eq(int one, int two) {
    return one == two;
}
int hash_int(int in) {
    return in;
}

DEFINE_GRAPH(int)

int_dynarray follow(int in) {
    int_dynarray result;
    result = int_dynarray_make(10);
    int i = (in + 1) % 5;
    while (i != in) {
        result = int_dynarray_add(result, i);
        i = (int) (((int)i + 1) %  5);
    }
    return result;
}

void print_int_graph(int_graph graph) {
    noderef i;
    noderef j;
    noderef_dynarray connections;
    for (i=0; i < graph.nodes.size; i++) {
        connections = int_graph_getchildren(graph, i);
        printf("%s", "\n Node: ");
        printf("%d", graph.nodes.begin[i]);
        printf("%s", " Connected To: ");
        for (j=0; j < connections.size; j++) {
            printf("%d", graph.nodes.begin[connections.begin[j]]);
            printf("%s", " ");
        }
    }
    return;
}

/*
int_graph rec_construct_int_graph(int_graph graph, 
                                  int_dynarray (*follow)(int),
                                  int_noderef_dict visited, 
                                  noderef current) {

    int_dynarray leads_dynarray = follow(int_graph_getnode(graph, current));

    noderef neighbor;
    noderef tmpref;
    int tmpval;
    size_t i;
    for (i=0; i < leads_dynarray.size; i++) {
        neighbor = int_noderef_dict_get(visited, leads_dynarray.begin[i]);
        if (neighbor == noderef_lookup_failure) {
            tmpval = leads_dynarray.begin[i];
            graph = int_graph_addnode(graph, tmpval, &tmpref);
            graph = int_graph_addedge(graph, current, tmpref);
            visited = int_noderef_dict_add(visited, int_noderef_bucket_make(tmpval, tmpref));
            graph = rec_construct_int_graph(graph,follow, visited, tmpref);
        }
        else {
            graph = int_graph_addedge(graph, current, neighbor);
        }
    }
    return graph;
}

int_graph construct_int_graph(int initial, int_dynarray (*follow)(int), size_t expectedsize) {
    int_graph result = int_graph_init(expectedsize);
    noderef tmp;
    result = int_graph_addnode(result, initial, &tmp);
    int_noderef_dict visited = int_noderef_dict_init(expectedsize * 4);
    visited = int_noderef_dict_add(visited, int_noderef_bucket_make(initial, tmp));
    result = rec_construct_int_graph(result, follow, visited, tmp);
    return result;
}
*/

DEFINE_CONSTRUCT_GRAPH(int)
    
int main() {
    int_graph graph = construct_int_graph(3, follow, 20);
    print_int_graph(graph);
    return 0;
}
