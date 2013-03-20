#include "dynarray.h"
#include "memoryman.h"

#ifndef TREE_DEFINED
#define TREE_DEFINED

/*Defines a generic tree and associated operations. A tree is implemented as a struct containing
the root's data and a dynamic array of the children of the root [which are of the same datatype
as the root itself]. */

#define DEFINE_TREE(type) \
/*This is where being able to declare the type of a dynarray and its methods separately \
comes in handy -- recursive types! */\
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
 /*Initializes a new tree (root node) with the given data for the root*/\
inline static type##_tree type##_tree_init(type in) { \
    type##_tree result; \
    /*Tree is the magic number! */ \
    result.children = type##_tree_dynarray_make(3); \
    result.data = in; \
    return result; \
} \
/*Adds the child node given in the second arg to the given tree */ \
inline static type##_tree type##_tree_addchild(type##_tree in, type##_tree toadd) { \
    in.children = type##_tree_dynarray_add(in.children, toadd); \
    return in; \
} \
inline static void type##_tree_free(type##_tree in) { \
    type##_tree_dynarray_free(in.children); \
    return; \
} \
inline static void type##_tree_recfree(type##_tree in) { \
    size_t i; \
    for (i=0; i < in.children.size; i++) { \
        type##_tree_recfree(in.children.begin[i]); \
    } \
    type##_tree_free(in); \
    return; \
} \
inline static void type##_tree_dynarray_recfree(type##_tree_dynarray in) { \
    size_t i; \
    for (i=0; i < in.size; i++) { \
        type##_tree_recfree(in.begin[i]); \
    } \
    return; \
} \
\
/*Depth-first map on the tree in the first argument using the function in the second arg, which
is a function that conceptually takes the data at the current root node as the first argument, 
and the dynamic array of its children as the second argument, and returns what the new data at
the root node should be. The traversal is performed depth-first, left-to-right */\
static type##_tree type##_tree_dfmap(type##_tree in, type (*transform)(type, type##_tree_dynarray)) { \
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
/* Input: a tree. Output: a dynamic array of the data of the child nodes*/ \
inline static type##_dynarray type##_tree_get_childarray(type##_tree in) { \
    type##_dynarray result = type##_dynarray_make(in.children.size + 1); \
    size_t i = 0; \
    while (i < in.children.size) { \
        result = type##_dynarray_add(result, in.children.begin[i].data); \
        i++; \
    } \
    return result; \
} \
 \
 /*Same thing as type##_tree_dfmap, except it instead takes a function to conceptually "fold"
 over dynamic array of the children's data with the root's data as the "zero element" 
 The function pointer passed follows the same format as dynarray_foldl's second arg */\
static type##_tree type##_tree_dfmap_foldl(type##_tree in, type (*transform)(type, type)) { \
    type##_dynarray tmp; \
    if (in.children.size > 0) { \
       size_t i = 0; \
       while (i < in.children.size) { \
           in.children.begin[i] = type##_tree_dfmap_foldl(in.children.begin[i], transform); \
           i++; \
       } \
       tmp = type##_tree_get_childarray(in); \
       in.data = type##_dynarray_foldl(tmp, transform, in.data); \
       type##_dynarray_free(tmp); \
    } \
    return in; \
} \
 \
 /*Height-first map over a tree. The function pointer passed as the second argument conceptually
 takes a dynamic array of child trees, and the data at the root node, and returns a new dynamic
 array of children with updated data values. Performed in a top-down, left-to-right traversal*/\
static type##_tree type##_tree_hfmap(type##_tree in, type##_tree_dynarray (*transform)(type##_tree_dynarray, type)) { \
    in.children = transform(in.children, in.data); \
    size_t i = 0; \
    while (i < in.children.size) { \
          in.children.begin[i] = type##_tree_hfmap(in.children.begin[i], transform); \
          i++; \
    } \
    return in; \
}
 
#endif
