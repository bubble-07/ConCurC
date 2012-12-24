#include "primorder.h"

lexid_tree_dynarray prim_reorder(lexid_tree_dynarray children, lexid this) {
    size_t i = 0;
    while (i < children.size) {
        if (isPrim(children.begin[i].data)) {
            children = lexid_tree_dynarray_moveToBegin(i, children);
            return children;
        }
        i++;
    }
    return children;
}


parse_result primorder(parse_result in) {
    in.AST = lexid_tree_hfmap(in.AST, &prim_reorder);
    return in;
}
