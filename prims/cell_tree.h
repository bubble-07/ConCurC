#include "cell.h"
#include "../libs/dynarray.h"
#include "../libs/dynstring.h"

#ifndef CELL_TREE_DEFINED
#define CELL_TREE_DEFINED

typedef struct cell_tree_struct* cell_tree;
DEFINE_DYNARRAY(cell_tree)

void print_cell_tree(cell_tree in, string_dynarray backsymtable);
cell_tree cell_tree_init(cell in);
//Adds the specified child tree to the tree. NOTE: Undefined behavior if toadd is already in another tree
cell_tree cell_tree_addchild(cell_tree in, cell_tree toadd);
cell_tree cell_tree_addleaf(cell_tree in, cell leaf);
size_t cell_tree_numchildren(cell_tree in);
cell_tree cell_tree_child(cell_tree in, size_t i);
cell cell_tree_child_data(cell_tree in, size_t i);
cell_tree cell_tree_parent(cell_tree in);
cell cell_tree_data(cell_tree in);
cell_tree cell_tree_make_child_applicative(cell_tree in, size_t i);

#endif
