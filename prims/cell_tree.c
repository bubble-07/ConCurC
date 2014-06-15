#include "cell_tree.h"

struct cell_tree_struct {
    cell data;
    cell_tree parent;
    cell_tree_dynarray children;
    size_t height; //Height of the given tree
    int offset; //Offset of the given tree within its parent (-1 if no parent)
};

size_t max(size_t a, size_t b) {
    return (a > b) ? a : b;
}

//Sets the parent of incoming cell_tree to something different
//Helper function
void setparent(cell_tree in, cell_tree p) {
    in->parent = p;
    return;
}
//Helper function to add a child reference to a cell_tree
void addchild(cell_tree in, cell_tree c) {
    //Update the offset of the child tree to point to where it will be
    c->offset = in->children.size;
    //Add it to the list of children
    in->children = cell_tree_dynarray_add(in->children, c);
    //Update the height of the tree if c is the longest branch
    in->height = max(in->height, c->height + 1);
    return;
}

//Creates a new cell_tree from the given cell
cell_tree cell_tree_init(cell in) { 
    cell_tree result = memalloc(sizeof(struct cell_tree_struct));
    result->children = cell_tree_dynarray_make(1);
    result->data = in;
    result->offset = -1; //No parent yet
    result->parent = NULL;
    result->height = 0; //height of a tree with 1 elem: 0
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

CellType cell_tree_getkind(cell_tree in) {
    cell data = cell_tree_data(in);
    return data.kind;
}

//Returns true if the given cell is a leaf
int cell_tree_isleaf(cell_tree in) {
    return (cell_tree_numchildren(in) == 0);
}
//Returns true if the given cell is the root
int cell_tree_isroot(cell_tree in) {
    return (cell_tree_parent(in) == NULL);
}
int cell_tree_isexpr(cell_tree in) {
    return (cell_tree_getkind(in) == EXPRCELL);
}
int cell_tree_islambda(cell_tree in) {
    return (cell_tree_getkind(in) == LAMBDA);
}

int cell_tree_hasparent(cell_tree in) {
    return !cell_tree_isroot(in);
}
int cell_tree_isapplicative(cell_tree in) {
    if (cell_tree_hasparent(in)) {
        return (cell_tree_get_offset(in) == 0);
    }
    return 1; //It's the root, but say it's in the applicative position
}

type_ref cell_tree_get_type_ref(cell_tree in) {
    return get_cell_type_ref(cell_tree_data(in));
}
cell_tree cell_tree_set_type_ref(cell_tree in, type_ref t) {
    in->data.type = t;
    return in;
}


int cell_tree_get_offset(cell_tree in) {
    return in->offset;
}

//Gets the number of children of the current cell_tree
size_t cell_tree_numchildren(cell_tree in) {
    return in->children.size;
}
//Gets the child tree at the given index
cell_tree cell_tree_child(cell_tree in, size_t i) {
    return in->children.begin[i];
}
//Gets all the children of the given node
cell_tree_dynarray cell_tree_children(cell_tree in) {
    return in->children;
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
//Sets the data at the current node
cell_tree cell_tree_setdata(cell_tree in, cell data) {
    in->data = data;
    return in;
}
//Moves the specified child to the applicative position [child 0] and shifts everything over
cell_tree cell_tree_make_child_applicative(cell_tree in, size_t i) {
    //Swap the offsets of the two children in question
    int tmp = in->children.begin[0]->offset;
    in->children.begin[0]->offset = in->children.begin[i]->offset;
    in->children.begin[i]->offset = tmp;

    //Actually perform the move
    in->children = cell_tree_dynarray_moveToBegin(i, in->children);
    return in;
}

//Helper method to define "flatten"
cell_tree_dynarray cell_tree_recflatten(cell_tree in, cell_tree_dynarray result) {
    //Add the current node to our result
    result = cell_tree_dynarray_add(result, in);
    //Recursively add children
    size_t i;
    for (i=0; i < cell_tree_numchildren(in); i++) {
        result = cell_tree_recflatten(cell_tree_child(in, i), result);
    }
    return result;
}

cell_tree_dynarray cell_tree_flatten(cell_tree in) {
    return cell_tree_recflatten(in, cell_tree_dynarray_make(1));
}

inline size_t cell_tree_height(cell_tree in) {
    return in->height;
}

//Helper function to compare two pointers to trees by height, with offset as a secondary criteria.
//If one has greater height than two, then 1 will be returned (swap). If one has a lower height than two, return -1 (no swap)
//However, if both heights are equal, then return 1 (swap) if one has a greater offset than two (comes later in children)
int heightcompare(const void* one, const void* two) {
    cell_tree onetree = *(cell_tree*)one;
    cell_tree twotree = *(cell_tree*)two;
    if (cell_tree_height(onetree) > cell_tree_height(twotree)) {
        return 1;
    }
    else if (cell_tree_height(onetree) == cell_tree_height(twotree)) {
        if (cell_tree_get_offset(onetree) > cell_tree_get_offset(twotree)) {
            return 1;
        }
    }
    return -1;
}
    
//Traverse nodes in increasing order of height and transform
cell_tree cell_tree_bottom_up_transform(cell_tree in, cell (*transform)(cell_tree)) {
    cell_tree_dynarray treenodes = cell_tree_flatten(in);
    //Sort all of the nodes in the tree in increasing order of height
    treenodes = cell_tree_dynarray_sort(treenodes, &heightcompare);
    //Transform each node
    size_t i;
    for (i=0; i < treenodes.size; i++) {
        cell_tree_setdata(treenodes.begin[i], transform(treenodes.begin[i]));
    }
    return in; //Return the tree we transformed
}

void print_cell_tree(cell_tree in, nametable names) {
    cell data = cell_tree_data(in);
    if (data.kind == EXPRCELL || data.kind == LAMBDACELL) {
        printf("[");
        //print_type(data.type, names); //Annotate with type of expression //TODO: Fix this!
        printf("]( ");
        int i;
        for(i=0; i < cell_tree_numchildren(in); i++) {
            print_cell_tree(cell_tree_child(in, i), names);
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
        /* FIXME: This is broken, for some reason! */
        
        parameter* toprint = data.data;
        print_parameter_ptr(toprint, names);
        
        return;
    }
    if (data.kind == POLYMORPH) {
        //Print out its name
        polymorph_ptr toprint = data.data;
        print_polymorph_ptr_name(toprint, names);
    }
}

