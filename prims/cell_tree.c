#include "cell_tree.h"

struct cell_tree_struct {
    cell data;
    cell_tree parent;
    cell_tree_dynarray children;
};


//Sets the parent of incoming cell_tree to something different
//Helper function
void setparent(cell_tree in, cell_tree p) {
    in->parent = p;
    return;
}
//Helper function to add a child reference to a cell_tree
void addchild(cell_tree in, cell_tree c) {
    in->children = cell_tree_dynarray_add(in->children, c);
    return;
}

//Creates a new cell_tree from the given cell
cell_tree cell_tree_init(cell in) { 
    cell_tree result = memalloc(sizeof(struct cell_tree_struct));
    result->children = cell_tree_dynarray_make(1);
    result->data = in;
    result->parent = NULL;
    return result;
}

//TODO: Implement "free" methods

cell_tree cell_tree_addchild(cell_tree in, cell_tree toadd) {
    setparent(toadd, in);
    addchild(in, toadd);
    return in;
}

//Adds a leaf node to the given tree
cell_tree cell_tree_addleaf(cell_tree in, cell leaf) {
    cell_tree leaftree = cell_tree_init(leaf);
    return cell_tree_addchild(in, leaftree);
}

//Gets the number of children of the current cell_tree
size_t cell_tree_numchildren(cell_tree in) {
    return in->children.size;
}
//Gets the child tree at the given index
cell_tree cell_tree_child(cell_tree in, size_t i) {
    return in->children.begin[i];
}
//Gets the data of the child tree at the given index
cell cell_tree_child_data(cell_tree in, size_t i) {
    return in->children.begin[i]->data;
}
//Gets the parent of the current tree (NULL for none)
cell_tree cell_tree_parent(cell_tree in) {
    return in->parent;
}
//Gets the data at the current node
cell cell_tree_data(cell_tree in) {
    return in->data;
}
//Moves the specified child to the applicative position [child 0] and shifts everything over
cell_tree cell_tree_make_child_applicative(cell_tree in, size_t i) {
    in->children = cell_tree_dynarray_moveToBegin(i, in->children);
    return in;
}

void print_cell_tree(cell_tree in, string_dynarray backsymtable) {
    cell data = cell_tree_data(in);
    if (data.kind == EXPRCELL || data.kind == LAMBDACELL) {
        printf("( ");
        int i;
        for(i=0; i < cell_tree_numchildren(in); i++) {
            print_cell_tree(cell_tree_child(in, i), backsymtable);
            printf(" ");
        }
        printf(")");
        return;
    }
    if (data.kind == INTCELL) {
        int* ptr = data.data;
        int val = *ptr;
        printf("%d", val);
        return;
    }
    if (data.kind == PARAMETER) {
        parameter* toprint = data.data;
        print_parameter_ptr(toprint, backsymtable);
        return;
    }
}

