#include "dynarray.h"
#include "memoryman.h"

#ifndef TREE_DEFINED
#define TREE_DEFINED

#define DEFINE_TREE(type) \
typedef struct type##_tree type##_tree; \
 \
DEFINE_DYNARRAY_TYPE(type##_tree); \
 \
struct type##_tree { \
    type data; \
    type##_tree_dynarray children; \
}; \
 \
DEFINE_DYNARRAY_METHODS(type##_tree); \
 \
 \
 \
type##_tree type##_tree_init(type in) { \
    type##_tree result; \
    result.children = type##_tree_dynarray_make(3); \
    result.data = in; \
    return result; \
} \
type##_tree type##_tree_addchild(type##_tree in, type##_tree toadd) { \
    in.children = type##_tree_dynarray_add(in.children, toadd); \
    return in; \
} \
type##_tree type##_tree_dfmap(type##_tree in, type (*transform)(type, type##_tree_dynarray)) { \
     if (in.children.size > 0) { \
         size_t i = 0; \
         while (i < in.children.size) { \
             in.children.begin[i] = type##_tree_dfmap(in.children.begin[i], transform); \
             i++; \
         } \
     } \
     in.data = transform(in.data, in.children); \
     return in; \
} \
 \
type##_dynarray type##_tree_get_childarray(type##_tree in) { \
    type##_dynarray result = type##_dynarray_make(in.children.size + 1); \
    size_t i = 0; \
    while (i < in.children.size) { \
        result = type##_dynarray_add(result, in.children.begin[i].data); \
        i++; \
    } \
    return result; \
} \
 \
 \
type##_tree type##_tree_dfmap_foldl(type##_tree in, type (*transform)(type, type)) { \
    if (in.children.size > 0) { \
       size_t i = 0; \
       while (i < in.children.size) { \
           in.children.begin[i] = type##_tree_dfmap_foldl(in.children.begin[i], transform); \
           i++; \
       } \
       in.data = type##_dynarray_foldl(type##_tree_get_childarray(in), transform, in.data); \
    } \
    return in; \
} \
 \
type##_tree type##_tree_hfmap(type##_tree in, type##_tree_dynarray (*transform)(type##_tree_dynarray, type)) { \
    in.children = transform(in.children, in.data); \
    size_t i = 0; \
    while (i < in.children.size) { \
          in.children.begin[i] = type##_tree_hfmap(in.children.begin[i], transform); \
          i++; \
    } \
    return in; \
}
 
#endif
