#include "digraph.h"
DEFINE_GRAPH(float)
int main() {
    float_graph test = float_graph_init(1);
    size_t i;
    size_t s = 384;
    for (i=0; i < s; i++) {
        test = float_graph_addnode(test, i);
    }
    for (i=0; i < s-2; i++) {
        test = float_graph_addedge(test, i, i + 1);
        test = float_graph_addedge(test, i, i + 2);
    }
    //flag_mat_print(test.adjmat);
    test = float_graph_transitiveclosure(test);
    //printf("%s", "COMPLETE! \n");
    //flag_mat_print(test.adjmat);
    float_graph_free(test);
    return 0;
}
