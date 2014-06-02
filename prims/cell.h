#include "../libs/tree.h"
#include "../libs/memoryman.h"
#include "../libs/dynstring.h"
#include "parameter.h"
#include "../libs/filehandler.h"

#ifndef CELL_DEFINED
#define CELL_DEFINED

typedef enum {
    FUNCTION, //A single function
    POLYMORPH, //Multiple functions with the same name
    INTCELL,
    STRINGCELL,
    FLOATCELL,
    PARAMETER, //Param used within the body of a function (references parameter)
    EXPRCELL, //Compound expression cell
    LAMBDACELL //Cell representing a lambda expression (children in the tree comprise the body)
} CellType;

/* Overview of basic cell kinds:

Function: a forwarding node that references a "function" struct
Polymorph: a forwarding node that references a "polymorph" struct
Integer: A node (Int type) that references a raw C integer
String: A node (String type) that references a dynstring
Float: "" float
Variable: A node that references a parameter
*/

typedef struct {
    CellType kind;
    void* data;
    fileLoc loc;
} cell;

DEFINE_DYNARRAY(cell)

DEFINE_TREE(cell)

static cell make_expr_cell() {
    cell result;
    result.kind = EXPRCELL;
    result.data = NULL;
    return result;
}

//Returns 1 if the given cell is some kind of function. 0 otherwise
static int cell_is_callable(cell in) {
    return (in.kind == LAMBDACELL || in.kind == POLYMORPH || in.kind == FUNCTION);
}


//Define a handy macro to make cells containing primitive datatypes.
//Cell kind is the kind of cell that stores the datatype, and type is
//the type it's internally represented by
#define DEF_MAKE_CELL(cellkind, type) \
static cell make_##type##_cell(type in) { \
    cell result; \
    result.kind = cellkind; \
    /*Copy the input to a new memory location*/ \
    type * ptr = memalloc(sizeof(type)); \
    *ptr = in; \
    result.data = (void*) ptr; \
    return result; \
}

DEF_MAKE_CELL(INTCELL, int)
DEF_MAKE_CELL(FLOATCELL, float)
DEF_MAKE_CELL(STRINGCELL, string)

//Define a handy macro for cells we make by their pointers
//And give the resulting function "name"
#define DEF_MAKE_CELL_FROM_REF(cellkind, type, name) \
static cell make_##name##_cell(type * in) { \
    cell result;  \
    result.kind = cellkind; \
    result.data = (void*) in; \
    return result; \
}

//NOTE: in order to avoid circular reference, we have to define a polymorph prototype here
struct polymorph;

DEF_MAKE_CELL_FROM_REF(POLYMORPH, struct polymorph, polymorph)
DEF_MAKE_CELL_FROM_REF(PARAMETER, parameter, parameter)

static void print_cell_tree(cell_tree in, string_dynarray backsymtable) {
    if (in.data.kind == EXPRCELL) {
        printf("( ");
        int i;
        for(i=0; i < in.children.size; i++) {
            print_cell_tree(in.children.begin[i], backsymtable);
            printf(" ");
        }
        printf(")");
        return;
    }
    if (in.data.kind == INTCELL) {
        int* ptr = in.data.data;
        int val = *ptr;
        printf("%d", val);
        return;
    }
    if (in.data.kind == PARAMETER) {
        parameter* toprint = in.data.data;
        print_parameter_ptr(toprint, backsymtable);
        return;
    }
}

#endif
