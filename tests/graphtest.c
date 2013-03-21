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

int_set follow(int in) {
    int_set result;
    result = int_set_init(10);
    int i = (in + 1) % 5;
    printf("%s", "starting... \n");
    printf("%d", in);
    while (i != in) {
        printf("%d", i);
        printf("%s", "\n");
        result = int_set_add(result, i);
        i = (int) (((int)i + 1) %  5);
    }
    printf("%s", "done! \n");
    return result;
}

noderef find(int_graph graph, int tofind) {
    noderef i;
    for (i=0; i < graph.nodes.size; i++) {
        if (graph.nodes.begin[i] == tofind) {
            return i;
        }
    }
    printf("%s", "ERROR! NOT FOUND");
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

int_graph rec(int_graph part, noderef current) {

    noderef_dynarray connections = int_graph_getchildren(part, current);
    if (connections.size > 0) {
        return part; //we must've already visited this node!
    }
    int currentval = int_graph_getnode(part, current);
    int_set leads = follow(currentval);
    int_dynarray leads_dynarray;
    leads_dynarray = int_set_to_dynarray(leads);

    noderef neighbor;
    noderef tmp;
    size_t i;
    for (i=0; i < leads_dynarray.size; i++) {
        neighbor = find(part, leads_dynarray.begin[i]);
        if (neighbor == -1) {
            part = int_graph_addnode(part, leads_dynarray.begin[i], &tmp);
            part = int_graph_addedge(part, current, tmp);
        }
        else {
            part = int_graph_addedge(part, current, neighbor);
        }
    }
    for (i=0; i < leads_dynarray.size; i++) {
        part = rec(part, find(part, leads_dynarray.begin[i]));
    }
    return part;
}
    
int main() {
    int_graph graph = int_graph_init(7);
    noderef tmp;
    graph = int_graph_addnode(graph, 3, &tmp);
    graph = rec(graph, tmp);
    flag_mat_print(graph.adjmat);
    print_int_graph(graph);
    return 0;
}
