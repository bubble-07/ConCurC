//Represents the best available information about a type [as it's being refined]
//Uses a sum type to do so. TODO: Use the internal "Either" representation for these! (Will simplify)

#include "polytype.h"

#ifndef TYPEINFO_DEFINED
#define TYPEINFO_DEFINED

typedef struct {
    polytype_dynarray options; //List of potential types
} TypeInfo;

#include "type_graph.h"

TypeInfo make_typeinfo(polytype_dynarray in);
void print_type(TypeInfo in, nametable names);
TypeInfo make_empty_type(); //Makes a new type that can't be anything
TypeInfo copy_type(TypeInfo in);
TypeInfo add_type(TypeInfo in, polytype a);
TypeInfo make_simple_type(polytype in);
TypeInfo make_simple_monotype(TypeGraphRef in);
TypeInfo make_unknown_type();
int type_eq(TypeInfo one, TypeInfo two);
void free_type(TypeInfo in);
TypeInfo concat_types(TypeInfo one, TypeInfo two);
TypeInfo intersect_types(polytype a, polytype b);
TypeInfo simplify_TypeInfo(TypeInfo in);
TypeInfo restrict_type(TypeInfo in, polytype a);
TypeInfo restrict_sum(TypeInfo in, TypeInfo by);
int type_is_empty(TypeInfo one);
int types_are_disjoint(TypeInfo one, TypeInfo two);

TypeInfo polytype_dynarray_to_TypeInfo(polytype_dynarray in);

#endif
