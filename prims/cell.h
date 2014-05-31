#include "../libs/tree.h"
#include "../libs/memoryman.h"
#include "../libs/dynstring.h"
#include "parameter.h"

#ifndef CELL_DEFINED
#define CELL_DEFINED

typedef enum {
    FUNCTION, //A single function
    POLYMORPH, //Multiple functions with the same name
    INTCELL,
    STRINGCELL,
    FLOATCELL,
    PARAMETER, //Param used within the body of a function (references parameter)
    DUMMY,
    EXPRCELL //Compound expression cell
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
} cell;

DEFINE_DYNARRAY(cell)

DEFINE_TREE(cell)

static cell get_dummy_cell() {
    cell result;
    result.kind = DUMMY;
    result.data = NULL;
    return result;
}

static cell make_expr_cell() {
    cell result;
    result.kind = EXPRCELL;
    result.data = NULL;
    return result;
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

#endif
