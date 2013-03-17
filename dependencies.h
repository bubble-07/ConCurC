#include "libs/digraph.h"

typedef struct {
    int checked; //predicate that is 0 if the path hasn't been checked and 1 if it has
    int root; //0 is the universal "libs" directory, 1 is the separated namespace

} moduleref;

DEFINE_GRAPH(
