#include "tree.h"

#define DEFINE_TRANSFORMS(typeone, typetwo) \
\
static typetwo##_tree typeone##_to_##typetwo##_tree_map(typeone##_tree in, typetwo (*trans)(typeone)) \
{ \
    typetwo##_tree result = typetwo##_tree_init(trans(in.data)); \
    if (in.children.size > 0) { \
        size_t i = 0; \
        while (i < in.children.size) { \
           result = typetwo##_tree_addchild(result, typeone##_to_##typetwo##_tree_map(in.children.begin[i], trans)); \
           i++; \
        } \
    }  \
    return result; \
}
            
