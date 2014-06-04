//Represents the pass that converts the program
//from a lexid_tree (in parse_result) to a collection 
//of functions, each of which contains a cell_tree representing
//function bodies.

#include "parser.h"
#include "collectnames.h"
#include "../prims/cell_tree.h"
#include "../prims/env.h"

#ifndef TOCELLS_DEFINED
#define TOCELLS_DEFINED

//For now, conceptually represent everything in the global function
//table.
void to_cells(parse_result in);
cell_tree convert_to_cells(lexid_tree in, env e);


#endif
