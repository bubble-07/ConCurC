//Define type equations and supporting architecture

typedef struct {
    polymorph_ptr poly;
    type_ref_dynarray args;
} is_polymorph;
//Defines a structure for the RH side of an equation
//that says a given type is a polymorphic function
//with the given argument types

typedef struct {
    int pos;
    type_ref func;
} is_in_pos;
//Defines a RH side that says a given type must fit
//in the specified position of a given function type

typedef struct {
    type_ref func;
    type_ref_dynarray args;
} is_result_of;
//Says that a given type is the result returned from
//applying func to args

typedef struct {
    type_ref t;
} is_subtype;
//Says a given type is a subtype of another

typedef struct {
    type_ref t;
} is_equal;
//Says a given type is the same as another
    


typedef union {
    is_polymorph is_polymorph_entry;
    is_in_pos is_in_pos_entry;
    is_result_of is_result_of_entry;
    is_subtype is_subtype_entry;
    is_equal is_equal_entry;
} type_expr;
//Poor-man's ADT for possible RH sides of type equations

typedef enum {
    is_polymorph_kind;
    is_in_pos_kind;
    is_result_of_kind;
    is_subtype_kind;
    is_equal_kind;
} type_expr_kind;
//Corresponding enum tags for possible RH sides


typedef struct {
    type_ref var;
    type_expr expr;
    type_expr_kind expr_kind;
} type_equation;
