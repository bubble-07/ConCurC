//Represents the pass that converts the program
//from a lexid_tree (in parse_result) to a collection 
//of functions, each of which contains a cell_tree representing
//function bodies.

#include "parser.h"
#include "collectnames.h"
#include "../prims/cell_tree.h"
#include "../prims/env.h"
#include "../prims/polymorph.h"
#include "../prims/function_table.h"

#ifndef TOCELLS_DEFINED
#define TOCELLS_DEFINED

//Structure to represent a list of polymorphs and a backsymtable
//as a way to modularize results of this pass.
typedef struct {
    polymorph_ptr_dynarray funcs;
    nametable names;
} def_collection; //Collection of definitions
    

def_collection to_cells(collectnames_result in);
cell_tree convert_to_cells(lexid_tree in, env e, function_table table);


#endif
