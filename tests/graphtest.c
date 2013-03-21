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
DEFINE_SET(int)

DEFINE_DICT(int, noderef)

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

noderef find(int_graph graph, int tofind) {
    noderef i;
    for (i=0; i < graph.nodes.size; i++) {
        if (graph.nodes.begin[i] == tofind) {
            return i;
        }
    }
    return -1;
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

//elems is the current set of all elements in the graph

int_graph rec(int_graph part, int_noderef_dict visited, noderef current) {

    int_dynarray leads_dynarray = follow(int_graph_getnode(part, current));

    noderef neighbor;
    noderef tmpref;
    int tmpval;
    size_t i;
    for (i=0; i < leads_dynarray.size; i++) {
        neighbor = int_noderef_dict_get(visited, leads_dynarray.begin[i]);
        if (neighbor == noderef_lookup_failure) {
            tmpval = leads_dynarray.begin[i];
            part = int_graph_addnode(part, tmpval, &tmpref);
            part = int_graph_addedge(part, current, tmpref);
            visited = int_noderef_dict_add(visited, int_noderef_bucket_make(tmpval, tmpref));
            part = rec(part,visited, tmpref);
        }
        else {
            part = int_graph_addedge(part, current, neighbor);
        }
    }
    return part;
}
    
int main() {
    int_graph graph = int_graph_init(7);
    noderef tmp;
    graph = int_graph_addnode(graph, 3, &tmp);
    int_noderef_dict visited = int_noderef_dict_init(100);
    visited = int_noderef_dict_add(visited, int_noderef_bucket_make(3, tmp));

    graph = rec(graph, visited, tmp);
    print_int_graph(graph);
    return 0;
}
