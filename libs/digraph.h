/* Implementation of an unweighted graph -- n^3 transitive closure, etc */
#include "flagmat.h"   
#include "set.h"
#ifndef GRAPHDEFINED
#define GRAPHDEFINED
typedef size_t noderef;
static const noderef noderef_lookup_failure = -1;
DEFINE_DYNARRAY(noderef)
DEFINE_SET(noderef)
#endif

#ifndef COLORFLAG_DEFINED
#define COLORFLAG_DEFINED
typedef char colorflag;
static const colorflag BLACK = 0;
static const colorflag GREY = 1;
static const colorflag WHITE = 2;
DEFINE_DYNARRAY(colorflag)
typedef struct {
    colorflag_dynarray marks;
    noderef_dynarray result;
} topo_partial;
#endif

#define DEFINE_GRAPH(type) \
DEFINE_DYNARRAY(type) \
\
typedef struct {\
    flag_mat adjmat;\
    type##_dynarray nodes;\
    size_t size;\
    size_t rsize;\
} type##_graph;\
\
static inline type##_graph type##_graph_init(size_t expectednumnodes) {\
    type##_graph result;\
    result.adjmat = flag_mat_init(expectednumnodes, expectednumnodes);\
    result.nodes = type##_dynarray_make(expectednumnodes + 2);\
    result.size = 0;\
    result.rsize = expectednumnodes;\
    return result;\
}\
static inline type type##_graph_getnode(type##_graph in, noderef current) {\
    return in.nodes.begin[current]; \
}\
static inline type##_graph type##_graph_setnode(type##_graph in, noderef current, type val) {\
    in.nodes.begin[current] = val; \
    return in; \
} \
\
static inline type##_graph type##_graph_addnode(type##_graph in, type toadd, noderef* loc) {\
    in.nodes = type##_dynarray_add(in.nodes, toadd);\
    if (!(in.size < in.rsize)) {\
        in.adjmat = flag_dynarray_dynarray_add(in.adjmat, flag_dynarray_zeroedrow(in.size));\
        in.rsize += 1;\
    }\
    *loc = in.nodes.size - 1; \
    in.adjmat = flag_mat_addzeroedcolumn(in.adjmat);\
    in.size += 1;\
    return in;\
}\
static inline type##_graph type##_graph_addedge(type##_graph in, noderef n1, noderef n2) {\
    in.adjmat.begin[n1].begin[n2] = FLAG_TRUE;\
    return in;\
}\
static inline type##_graph type##_graph_remove_edge(type##_graph in, noderef n1, noderef n2) {\
    in.adjmat.begin[n1].begin[n2] = FLAG_FALSE;\
    return in;\
} \
static inline int type##_graph_testedge(type##_graph in, noderef n1, noderef n2) {\
    return in.adjmat.begin[n1].begin[n2];\
}\
static inline noderef_dynarray type##_graph_getchildren(type##_graph in, noderef current) {\
    noderef_dynarray result = noderef_dynarray_make(in.size);\
    noderef i;\
    for (i=0; i < in.size; i++) {\
        if (in.adjmat.begin[current].begin[i] == FLAG_TRUE) {\
            result = noderef_dynarray_add(result, i);\
        }\
    }\
    return result;\
}\
static inline type##_graph type##_graph_copy(type##_graph in) {\
    type##_graph result = in;\
    result.adjmat = flag_mat_copy(in.adjmat);\
    result.nodes = type##_dynarray_copy(in.nodes);\
    return result;\
}\
/* the following function switches the numbering/position of a given node -- WARNING: breaks \
 * references */ \
static inline type##_graph type##_graph_switch(type##_graph in, noderef one, noderef two) { \
    /*swap the data in the nodes array*/ \
    type tmp = type##_graph_getnode(in, one); \
    in = type##_graph_setnode(in, one, type##_graph_getnode(in, two)); \
    in = type##_graph_setnode(in, two, tmp); \
    in.adjmat = flag_mat_swapboth(in.adjmat, one, two); \
    return in; \
} \
/* the following "detaches" a node, disconnecting it completely */ \
static inline type##_graph type##_graph_detach(type##_graph in, noderef one) { \
    noderef i; \
    for (i=0; i < in.size; i++) { \
        in = type##_graph_remove_edge(in, i, one); \
        in = type##_graph_remove_edge(in, one, i); \
    } \
    return in; \
} \
/* finds all of the nodes with no incoming/outgoing connections, removes them */ \
static inline type##_graph type##_graph_gc(type##_graph in, void (*destructor)(type)) { \
    noderef i; \
    noderef j; \
    int zeroed; \
    size_t initsize = in.size; \
    for (i=0; i < in.size; i++) { \
        zeroed = 1; \
        for (j = 0; j < in.size && zeroed; j++) { \
            if (type##_graph_testedge(in, i, j) || type##_graph_testedge(in, j, i)) { \
                zeroed = 0; \
            } \
        } \
        if (zeroed) {  \
            /*Hey, look -- node i is one that can be gc'ed*/ \
            in = type##_graph_switch(in, i, in.size - 1); \
            in.size -= 1; \
            i -= 1; \
        } \
    } \
    for (i=(in.size - 1); i < initsize; i++) { \
        destructor(in.nodes.begin[i]); \
    } \
    return in; \
} \
 \
 \
         \
\
static inline type##_graph type##_graph_floydwarshall(type##_graph in) {\
    size_t i;\
    size_t j;\
    size_t k;\
    for (k=0; k < in.size; k++) {\
        for (i=0; i < in.size; i++) {\
            for (j=0; j < in.size; j++) {\
                if (type##_graph_testedge(in, i, k) & type##_graph_testedge(in, k, j)) {\
                    in = type##_graph_addedge(in, i, j);\
                } /* \
                in.adjmat.begin[i].begin[j] = \
                    in.adjmat.begin[i].begin[k] & in.adjmat.begin[k].begin[j] | \
                    in.adjmat.begin[i].begin[j];\ */ \
            }\
        }\
    }\
    return in;\
}\
static inline type##_graph type##_graph_transitiveclosure(type##_graph in) {\
    in = type##_graph_floydwarshall(in);\
    return in;\
}\
static inline topo_partial type##_graph_topo_sort_r(type##_graph in, noderef current, \
                                                                    topo_partial state) { \
    noderef_dynarray tmpchildren = type##_graph_getchildren(in, current); \
    size_t i; \
    for (i=0; i < tmpchildren.size; i++) { \
        if (state.marks.begin[tmpchildren.begin[i]] == WHITE) { \
            state.marks.begin[tmpchildren.begin[i]] = GREY; \
            state = type##_graph_topo_sort_r(in, tmpchildren.begin[i], state); \
        } \
    } \
    state.marks.begin[current] = BLACK; \
    state.result = noderef_dynarray_add(state.result, current); \
    return state; \
} \
static inline noderef_dynarray type##_graph_get_roots(type##_graph in) {\
    noderef_dynarray result = noderef_dynarray_make(1); \
    noderef from; \
    noderef to; \
    for (to = 0; to < in.size; to++) { \
        int can_be_root = 1; /*by default, assume it can be root*/ \
        for (from = 0; from < in.size && can_be_root; from++) { \
            if (type##_graph_testedge(in, from, to)) { \
                can_be_root = 0; /*can't be a root!*/ \
            } \
        } \
        if (can_be_root) { \
            result = noderef_dynarray_add(result, to); /* it is a root! */ \
        } \
    } \
    return result; \
} \
 \
static inline noderef_dynarray type##_graph_topo_sort(type##_graph in) {\
    topo_partial state; \
    state.result = noderef_dynarray_make(in.size); \
    state.marks = colorflag_dynarray_make(in.size); \
    state.marks = colorflag_dynarray_fill(state.marks, WHITE, in.size); \
    noderef_dynarray roots = type##_graph_get_roots(in); \
    size_t i; \
    for (i = 0; i < roots.size; i++) { \
        state = type##_graph_topo_sort_r(in, roots.begin[i], state); \
    } \
    colorflag_dynarray_free(state.marks); \
    return state.result; \
} \
\
\
static inline type##_graph type##_graph_free(type##_graph in) {\
    type##_dynarray_free(in.nodes);\
    flag_mat_free(in.adjmat);\
    return;\
}

//must first define type##_eq and hash##_type. 
#define DEFINE_CONSTRUCT_GRAPH(type) \
DEFINE_DICT(type, noderef) \
\
static type##_graph rec_construct_##type##_graph(type##_graph graph, \
                                  type##_dynarray (*follow)(type),\
                                  type##_noderef_dict visited, \
                                  noderef current) {\
\
    type##_dynarray leads_dynarray = follow(type##_graph_getnode(graph, current));\
\
    noderef neighbor;\
    noderef tmpref;\
    type tmpval;\
    size_t i;\
    for (i=0; i < leads_dynarray.size; i++) {\
        neighbor = type##_noderef_dict_get(visited, leads_dynarray.begin[i]);\
        if (neighbor == noderef_lookup_failure) {\
            tmpval = leads_dynarray.begin[i];\
            graph = type##_graph_addnode(graph, tmpval, &tmpref);\
            graph = type##_graph_addedge(graph, current, tmpref);\
           visited = type##_noderef_dict_add(visited, type##_noderef_bucket_make(tmpval, tmpref));\
            graph = rec_construct_##type##_graph(graph,follow, visited, tmpref);\
        }\
        else {\
            graph = type##_graph_addedge(graph, current, neighbor);\
        }\
    }\
    return graph;\
}\
\
/* Constructs a directed graph from the given initial value, a generating function, and \
 * the expected size of the resulting graph. */ \
static type##_graph construct_##type##_graph(type initial, type##_dynarray (*follow)(type), \
                                      size_t expectedsize) {\
    type##_graph result = type##_graph_init(expectedsize);\
    noderef tmp;\
    result = type##_graph_addnode(result, initial, &tmp);\
    type##_noderef_dict visited = type##_noderef_dict_init(expectedsize * 4);\
    visited = type##_noderef_dict_add(visited, type##_noderef_bucket_make(initial, tmp));\
    result = rec_construct_##type##_graph(result, follow, visited, tmp);\
    return result;\
}

#define DEFINE_MERGE_CYCLES(type) \
static type##_graph type##_graph_collapse(type##_graph in, type (*merge)(type##_dynarray), \
                                                           noderef_dynarray tocollapse) { \
    if (tocollapse.size < 2) { \
        return in; \
    } \
    type##_dynarray args = type##_dynarray_make(tocollapse.size); \
    args = type##_dynarray_add(args, in.nodes.begin[tocollapse.begin[0]]); \
    size_t i; \
    for (i=1; i < tocollapse.size; i++) { \
        args = type##_dynarray_add(args, in.nodes.begin[tocollapse.begin[i]]); \
        in = type##_graph_detach(in, tocollapse.begin[i]); \
    } \
    in.nodes.begin[tocollapse.begin[0]] = merge(args);  \
    return in; \
}
#define DEFINE_BLAH \
/*Returns a sub-list of the incoming nodes that only contains the cycle referenced by the last \
* arg. TODO: Change test for equality to be overloadable. */ \
static noderef_dynarray type##_graph_find_cycle_elems(noderef_dynarray in, noderef root) { \
    noderef_dynarray result = noderef_dynarray_make(1); \
    result = noderef_dynarray_add(result, root); \
    size_t i; \
    for (i = in.size - 1; i > -1; i--) { \
        if (in.begin[i] == root) { \
            i = -1; \
        } \
        else { \
            result = noderef_dynarray_add(result, in.begin[i]); \
        } \
    } \
    return result; \
} \
 \
 \
static noderef_set type##_graph_condense_r(type##_graph in, type (*merge)(type##_dynarray), \
                                                noderef current, colorflag_dynarray marks \
                                                noderef_dynarray stack, noderef_set toremove) {\
    stack = noderef_dynarray_add(stack, current);  \
    marks.begin[current] = GREY; \
    noderef_dynarray children = type##_graph_getchildren(in, current); \
    size_t i; \
    for (i = 0; i < children.size; i++) { \
        switch (marks.begin[children.begin[i]]) { \
            case WHITE: \
                toremove = type##_graph_condense_r(in, merge, children.begin[i], marks, stack); \
                break; \
            default: \
                /* there's some kind of cycle*/ \
                noderef_dynarray tmp = type##_graph_find_cycle_elems(stack, children.begin[i]); \
                in = type##_graph_collapse(in, merge, tmp); \
                 \
                 \
    }
