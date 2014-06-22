#include "type.h"
#include "nametable.h"

#ifndef TYPE_REF_DEFINED
#define TYPE_REF_DEFINED

//DESCRIPTION:
//Defines type_refs, which are to be interpreted as
//references to a uniform type variable. That is, if
//the collection of all types being inferred in a program
//forms a disjoint union, a type_ref represents a reference
//to one of the disjoint sets. All inference is performed
//through unification on initially separate sets according
//to rules (type equations) associated with each set

//Further, each set can either have a "concrete type",
//or it can be a type variable (for parametric polymorphism)
//Everything is a type variable until proven concrete.

/* IMPLEMENTATION:
Currently, we will just use a simple tree implementation.
TODO: Use proper algorithms for this */


typedef enum {
    type_ref_forwarding, //Forwarding node (pointer references another node)
    type_ref_representative //Representative node (pointer references information structure)
} type_ref_kind;

typedef struct {
    void* data; //Could be a pointer to the information struct or another type_ref
    type_ref_kind kind; //Whatever the case, denote it by this
} type_ref_node;

typedef type_ref_node* type_ref;

DEFINE_DYNARRAY(type_ref)

//Hash to same value iff have same representative node
int hash_type_ref(type_ref in);
//Equal iff have same representative node
int type_ref_eq(type_ref one, type_ref two);

type_ref make_empty_type_ref(); //Makes a new type ref that's completely empty
type_ref make_known_type_ref(TypeInfo in); //Makes a new type ref with a bounded type

TypeInfo type_ref_getbound(type_ref in);

//Restricts the incoming type_ref to fall under the given type
type_ref type_ref_restrict(type_ref in, TypeInfo info);

//Unifies the two type refs
type_ref unify_type_refs(type_ref one, type_ref two);

void print_type_ref(type_ref in, nametable names);
void print_type_ref_list(type_ref_dynarray in, nametable names);

#endif

//NOTE: for all of the methods involving type_equations, see type_ref_info.h
