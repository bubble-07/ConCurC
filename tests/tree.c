#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../tree.h"             

DEFINE_TREE(int)
//META 
      
void tree_print(int_tree in) {
     printf("%d", in.data);
     printf("%s", "\n");
     if (in.children.size > 0) {
         size_t i = 0;
         while (i < in.children.size) {
            tree_print(in.children.begin[i]);
            i++;
        }
     }
     return;
}

int addup(int selfval, int_tree_dynarray children) {
    if (children.size == 0) {
        return selfval;
    }
    size_t i = 0;
    while (i < children.size) {
        selfval += children.begin[i].data;
        i++;
    }
    return selfval;
}

int_tree_dynarray addDown(int_tree_dynarray children, int root) {
    size_t i = 0;
    while (i < children.size) {
          children.begin[i].data = root + children.begin[i].data;
          i++;
    }
    return children;
}

int add(int one, int two) { return one + two; }

int main(int argc, const char * argv[]) {
    int_tree test = int_tree_init(0);
    int_tree branch1 = int_tree_init(0);
    branch1 = int_tree_addchild(branch1, int_tree_init(4));
    branch1 = int_tree_addchild(branch1, int_tree_init(5));
    test = int_tree_addchild(test, branch1);
    test = int_tree_dfmap(test, &addup);
    tree_print(test);
    printf("%s", "\n");
    test = int_tree_hfmap(test, &addDown);
    tree_print(test);
    printf("%s", "\n");
    test = int_tree_dfmap_foldl(test, &add);
    tree_print(test);
    return 0;
}
