typedef enum {
    FUNCTION, //A single function
    POLYMORPH, //Multiple functions with the same name
    INTEGER,
    STRING,
    FLOAT,
    PARAMETER, //Parameter: to a function
    VARIABLE, //Within the body of the function (references parameter)
    EXPRESSION //Denotes a compound expression
} CellType;

/* Overview of basic cell kinds:

Function: a forwarding node (null type) that references a "function" struct
Polymorph: a forwarding node (null type) that references a "polymorph" struct
Integer: A node (Int type) that references a raw C integer
String: A node (String type) that references a dynstring
Float: "" float
Parameter: A node (given type) that may reference some data
Variable: A node (null type) that references a parameter cell
Expression: A node (given type) that references a function, and a list of cells as arguments
*/

typedef struct {
    TypeInfo type;
    CellType kind;
    void* data;
} cell;

DEFINE_DYNARRAY(cell)
DEFINE_TREE(cell)
