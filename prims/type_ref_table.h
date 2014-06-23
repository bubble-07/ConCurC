//Defines a global table of type_refs
//Will be used for assigning unique names

#include "type_ref.h"
#include "type_equation.h"
#include "type_ref_info.h"
#include "nametable.h"

#ifndef TYPEREFTABLEDEFINED
#define TYPEREFTABLEDEFINED

typedef type_ref_dynarray type_ref_table;

type_ref_table type_ref_table_add(type_ref_table in, type_ref t);
type_ref_table type_ref_table_init();

void print_type_ref_table_equations(type_ref_table in, nametable names);

#endif
