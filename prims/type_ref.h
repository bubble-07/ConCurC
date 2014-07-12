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
DEFINE_DYNARRAY(type_ref_dynarray)

static const type_ref type_ref_lookup_failure = NULL;

//Hash to same value iff have same representative node
int hash_type_ref(type_ref in);

//Equal iff have same representative node
int type_ref_eq(type_ref one, type_ref two);

//Slightly different version of equality -- equal if have the same form and bounds
int type_ref_trivial_eq(type_ref one, type_ref two);

//Same as above, but elementwise on a list (used for comparing argument lists)
int type_ref_dynarray_trivial_eq(type_ref_dynarray one, type_ref_dynarray two);

type_ref make_unknown_type_ref(); //Makes a new type ref that's completely empty

//Unifies the two type refs
type_ref unify_type_refs(type_ref one, type_ref two);

//Makes type ref a become a reference to b (mutation) (if it's the principal node, dealloc the associated structure)
void type_ref_makepoint(type_ref a, type_ref b);

void print_type_ref(type_ref in, nametable names);
void print_type_ref_list(type_ref_dynarray in, nametable names);

#endif

//NOTE: for all of the methods involving type_equations, see type_ref_info.h
