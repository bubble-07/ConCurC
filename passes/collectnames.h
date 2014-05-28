#include "../prims/global_function_table.h"
#include "parser.h"

#ifndef COLLECTNAMES_DEFINED
#define COLLECTNAMES_DEFINED

/* This will populate the global function table with empty entries
as a side-effect. [TODO: modularize!] */

string get_name_from_def(lexid_tree_dynarray def_expr, string_dynarray backsymtable);
parse_result collectnames(parse_result in);

#endif
