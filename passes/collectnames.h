#include "../prims/global_function_table.h"
#include "parser.h"

#ifndef COLLECTNAMES_DEFINED
#define COLLECTNAMES_DEFINED

/* This will populate the global function table with empty entries
as a side-effect. [TODO: modularize!] */

lexid get_name_from_def(lexid_tree_dynarray def_expr);
parse_result collectnames(parse_result in);

#endif
