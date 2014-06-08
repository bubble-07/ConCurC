#include "../prims/function_table.h"
#include "parser.h"

#ifndef COLLECTNAMES_DEFINED
#define COLLECTNAMES_DEFINED

/* This will populate a function table with empty entries
using source from a given parse tree*/

typedef struct {
    parse_result parse;
    function_table table;
} collectnames_result; //Sort of an intermediate stage between lexids and cells


lexid get_name_from_def(lexid_tree_dynarray def_expr);
collectnames_result collectnames(parse_result in);

#endif
