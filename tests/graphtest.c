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

DEFINE_CONSTRUCT_GRAPH(int)
    
int main() {
    int_graph graph = construct_int_graph(3, follow, 20);
    print_int_graph(graph);
    return 0;
}
