/* Implementation of an unweighted graph -- n^3 transitive closure, etc */
#include "flagmat.h"   
#ifndef GRAPHDEFINED
#define GRAPHDEFINED
typedef size_t noderef;
static const noderef noderef_lookup_failure = -1;
DEFINE_DYNARRAY(noderef);
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
\
static inline type##_graph type##_graph_floydwarshall(type##_graph in) {\
    size_t i;\
    size_t j;\
    size_t k;\
    for (k=0; k < in.size; k++) {\
        for (i=0; i < in.size; i++) {\
            for (j=0; j < in.size; j++) {\
                /*\
                if (type##_graph_testedge(in, i, k) & type##_graph_testedge(in, k, j)) {\
                    in = type##_graph_addedge(in, i, j);\
                } */\
                in.adjmat.begin[i].begin[j] = \
                    in.adjmat.begin[i].begin[k] & in.adjmat.begin[k].begin[j] | \
                    in.adjmat.begin[i].begin[j];\
            }\
        }\
    }\
    return in;\
}\
static inline type##_graph type##_graph_transitiveclosure(type##_graph in) {\
    in = type##_graph_floydwarshall(in);\
    return in;\
}\
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
type##_graph rec_construct_##type##_graph(type##_graph graph, \
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
type##_graph construct_##type##_graph(type initial, type##_dynarray (*follow)(type), \
                                      size_t expectedsize) {\
    type##_graph result = type##_graph_init(expectedsize);\
    noderef tmp;\
    result = type##_graph_addnode(result, initial, &tmp);\
    type##_noderef_dict visited = type##_noderef_dict_init(expectedsize * 4);\
    visited = type##_noderef_dict_add(visited, type##_noderef_bucket_make(initial, tmp));\
    result = rec_construct_##type##_graph(result, follow, visited, tmp);\
    return result;\
}
