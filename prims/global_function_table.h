//A temporary solution to represent all functions as global
//[until we get proper scoping and modules working]

#include "function_table.h"

#ifdef GLOBALTABLE_DEFINED

extern function_table global_table;
#endif

#ifndef GLOBALTABLE_DEFINED
#define GLOBALTABLE_DEFINED

function_table global_table;


#endif
