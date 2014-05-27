#include "../libs/tree.h"

#ifndef CELL_DEFINED
#define CELL_DEFINED

typedef enum {
    FUNCTION, //A single function
    POLYMORPH, //Multiple functions with the same name
    INTEGERCELL,
    STRINGCELL,
    FLOATCELL,
    VARIABLE, //Param used within the body of a function (references parameter)
    DUMMY
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


#endif
