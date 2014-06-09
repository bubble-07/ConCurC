#include "cell.h"
#include "../libs/dynarray.h"
#include "../libs/dynstring.h"
#include "nametable.h"
#include "polymorph.h"

#ifndef CELL_TREE_DEFINED
#define CELL_TREE_DEFINED

typedef struct cell_tree_struct* cell_tree;
DEFINE_DYNARRAY(cell_tree)

void print_cell_tree(cell_tree in, nametable names);
cell_tree cell_tree_init(cell in);
//Adds the specified child tree to the tree. NOTE: Undefined behavior if toadd is already in another tree
cell_tree cell_tree_addchild(cell_tree in, cell_tree toadd);
cell_tree cell_tree_addleaf(cell_tree in, cell leaf);
size_t cell_tree_numchildren(cell_tree in);
cell_tree cell_tree_child(cell_tree in, size_t i);
cell_tree_dynarray cell_tree_children(cell_tree in);
cell cell_tree_child_data(cell_tree in, size_t i);
cell_tree cell_tree_parent(cell_tree in);

//Returns the offset [position] of the current cell tree in its parent
int cell_tree_get_offset(cell_tree in);

CellType cell_tree_getkind(cell_tree in);

int cell_tree_isleaf(cell_tree in);
int cell_tree_isroot(cell_tree in);
int cell_tree_isexpr(cell_tree in);
int cell_tree_islambda(cell_tree in);

cell cell_tree_data(cell_tree in);
cell_tree cell_tree_setdata(cell_tree in, cell data);
size_t cell_tree_height(cell_tree in);
cell_tree cell_tree_make_child_applicative(cell_tree in, size_t i);
//Get a flat list of references to every node in the tree (don't care about order)
cell_tree_dynarray cell_tree_flatten(cell_tree in);

//Traverses the cell_tree from the bottom up, in order of increasing node height
cell_tree cell_tree_bottom_up_transform(cell_tree in, cell (*transform)(cell_tree));

#endif
