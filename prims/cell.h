#include "../libs/memoryman.h"
#include "../libs/dynstring.h"
#include "parameter.h"
#include "../libs/filehandler.h"
#include "lambda.h"

#ifndef CELL_DEFINED
#define CELL_DEFINED

#include "type_ref.h"

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
    type_ref type;
    void* data;
    fileLoc loc;
} cell;

DEFINE_DYNARRAY(cell)

static type_ref get_cell_type_ref(cell in) {
    return in.type;
}

static cell make_expr_cell() {
    cell result;
    result.kind = EXPRCELL;
    result.data = NULL;
    result.type = make_empty_type_ref();
    return result;
}

//Returns 1 if the given cell is some kind of function. 0 otherwise
static int cell_is_callable(cell in) {
    return (in.kind == LAMBDACELL || in.kind == POLYMORPH || in.kind == FUNCTION);
}

/*
static cell update_cell_type(cell in, TypeInfo i) {
    in.type = i;
    return in;
}
static TypeInfo get_cell_type(cell in) {
    return in.type;
}
*/

//Define a handy macro to make cells containing primitive datatypes.
//Cell kind is the kind of cell that stores the datatype, and type is
//the type it's internally represented by
#define DEF_MAKE_CELL(cellkind, t) \
static cell make_##t##_cell(t in) { \
    cell result; \
    result.kind = cellkind; \
    /*Copy the input to a new memory location*/ \
    t * ptr = memalloc(sizeof(t)); \
    *ptr = in; \
    result.type = make_empty_type_ref(); \
    result.data = (void*) ptr; \
    return result; \
}

DEF_MAKE_CELL(INTCELL, int)
DEF_MAKE_CELL(FLOATCELL, float)
DEF_MAKE_CELL(STRINGCELL, string)

//Define a handy macro for cells we make by their pointers
//And give the resulting function "name"
#define DEF_MAKE_CELL_FROM_REF(cellkind, t, name) \
static cell make_##name##_cell(t * in) { \
    cell result;  \
    result.kind = cellkind; \
    result.data = (void*) in; \
    result.type = make_empty_type_ref(); \
    return result; \
}

//NOTE: in order to avoid circular reference, we have to define a polymorph prototype here
struct polymorph;

DEF_MAKE_CELL_FROM_REF(POLYMORPH, struct polymorph, polymorph)
DEF_MAKE_CELL_FROM_REF(PARAMETER, parameter, parameter)

//If the given cell is a parameter, gives a pointer to the referenced param
static parameter_ptr cell_get_parameter_ptr(cell in) {
    return (parameter*) in.data;
}

static struct polymorph* cell_get_polymorph_ptr(cell in) {
    return (struct polymorph*) in.data;
}

DEF_MAKE_CELL_FROM_REF(LAMBDACELL, lambda, lambda)

#endif
