#include "../libs/memoryman.h"
#include "type_ref.h"
#include "type_ref_info.h"

//Helper function to make a type ref from typical type info with no equations
type_ref make_type_ref(polytype bound, int parametric, int known) {
    type_ref_info* info = memalloc(sizeof(type_ref_info));
    info->upperbound = bound;
    info->parametric = parametric;
    info->known = known;
    info->equations = equation_set_init();
    type_ref_node* result = memalloc(sizeof(type_ref_node));
    result->data = info;
    result->kind = type_ref_representative; //This is the representative node
    return result;
}

//Makes a type_ref that is known and parametric with the given bound
type_ref make_known_parametric_type_ref(polytype in) {
    make_type_ref(in, 1, 1);
}

//Makes a completely unknown type_ref (up to inference)
type_ref make_unknown_type_ref() {
    return make_type_ref(make_unknown_type(), 1, 0); //Type_refs are parametric until proven otherwise
}

void type_ref_makepoint(type_ref a, type_ref b) {
    //TODO: If a is NOT forwarding to begin with, memory management!
    a->data = b;
    a->kind = type_ref_forwarding;
    return;
}

//Gets the representative node
type_ref find(type_ref in) {
    if (in->kind == type_ref_representative) {
        return in;
    }
    //Otherwise, follow the chain
    return find((type_ref) in->data);
}

type_ref type_ref_get_rep(type_ref in) {
    return find(in);
}

int hash_type_ref(type_ref in) {
    return (int) find(in); //Return address of representative
}

int type_ref_eq(type_ref one, type_ref two) {
    //Handle special case that both are null
    //(used for dictionary implementations)
    if (one == NULL && two == NULL) {
        return 1;
    }
    //Otherwise, if only one is null, return 0
    else if (one == NULL || two == NULL) {
        return 0;
    }
    return find(one) == find(two); //Equal iff representative addresses equal
}

//Helper to get info of type_ref
type_ref_info* getinfo(type_ref in) {
    type_ref rep = find(in);
    return (type_ref_info*) rep->data;
}

//Helper to set a type_ref's "data" field to point to given type_ref_info
type_ref setinfo(type_ref in, type_ref_info* info) {
    in->data = (void*) info;
    in->kind = type_ref_representative;
    return in;
}

//Given a list of type_refs, returns the respective pointers to type_ref info
//for each of them [scraped from representative nodes]
type_ref_info_ptr_dynarray get_type_ref_info_list(type_ref_dynarray in) {
    type_ref_info_ptr_dynarray result = type_ref_info_ptr_dynarray_make(1);
    int i;
    for (i=0; i < in.size; i++) {
        result = type_ref_info_ptr_dynarray_add(result, getinfo(in.begin[i]));
    }
    return result;
}

type_ref_info_ptr_dynarray restore_type_ref_info_list(type_ref_info_ptr_dynarray infos, type_ref_dynarray dest) {
    int i;
    for (i=0; i < infos.size; i++) {
        setinfo(dest.begin[i], infos.begin[i]);
    }
    return infos;
}

//Gets the principal bounding type of the given typeref
polytype type_ref_getbound(type_ref in) {
    type_ref_info* info = getinfo(in);
    return info->upperbound;
}

int type_ref_is_parametric(type_ref in) {
    type_ref_info* info = getinfo(in);
    return info->parametric;
}
int type_ref_is_simple(type_ref in) {
    return !type_ref_is_parametric(in);
}
int type_ref_is_known(type_ref in) {
    type_ref_info* info = getinfo(in);
    return info->known;
}
int type_ref_is_unknown(type_ref in) {
    return !type_ref_is_known(in);
}

type_ref type_ref_setknown(type_ref in, int known) {
    type_ref_info* info = getinfo(in);
    info->known = known;
    return in;
}

type_ref type_ref_setparametric(type_ref in, int parametric) {
    type_ref_info* info = getinfo(in);
    info->parametric = parametric;
    return in;
}

type_ref type_ref_setbound(type_ref in, polytype t) {
    type_ref_info* info = getinfo(in);
    info->upperbound = t;
    return in;
}

//Equal if the two type refs have the same form (that is, unknown variables equal unknown variables,
//and all others equal iff have the same upper bound)
//TODO: Modify this for type variables!
int type_ref_trivial_eq(type_ref one, type_ref two) {
    return type_eq(type_ref_getbound(one), type_ref_getbound(two));
}

int type_ref_dynarray_trivial_eq(type_ref_dynarray one, type_ref_dynarray two) {
    if (one.size != two.size) {
        return 0;
    }
    int i;
    for (i=0; i < one.size; i++) {
        if (!type_ref_trivial_eq(one.begin[i], two.begin[i])) {
            return 0;
        }
    }
    return 1;
}

//Adds the given type equation to the typeref
type_ref type_ref_add_equation(type_ref in, type_equation eqn) {
    type_ref rep = find(in);
    type_ref_info* info = rep->data;
    info->equations = equation_set_add(info->equations, eqn);
    return in;
}

type_equation_dynarray type_ref_get_equations(type_ref in) {
    type_ref rep = find(in);
    type_ref_info* info = rep->data;
    return info->equations;
}

polymorph_ptr type_ref_getpoly(type_ref in) {
    type_equation_dynarray eqns = type_ref_get_equations(in);
    int i;
    for (i=0; i < eqns.size; i++) {
        if (eqns.begin[i].expr_kind == is_polymorph_kind) {
            is_polymorph poly_eqn = get_poly_RH(eqns.begin[i]);
            return poly_eqn.poly;
        }
    }
    return NULL;
}


type_ref type_ref_addpoly_eqn(type_ref in, polymorph_ptr poly, typeslot_dynarray args) {
    return type_ref_add_equation(in, make_poly_eqn(poly, args));
}
type_ref type_ref_addargpos_eqn(type_ref in, typeslot func, int pos) {
    return type_ref_add_equation(in, make_argpos_eqn(pos, func));
}
type_ref type_ref_addapply_eqn(type_ref in, typeslot func, typeslot_dynarray args) {
    return type_ref_add_equation(in, make_apply_eqn(func, args));
}

int type_ref_restrict(type_ref in, polytype bound) {
    type_ref rep = find(in);
    type_ref_info* info = rep->data;

    polytype oldbound = copy_type(info->upperbound);

    info->upperbound = intersect_types(info->upperbound, bound);
    return !type_eq(oldbound, info->upperbound); //Active if the upper bound has changed
}

int type_refs_same_class(type_ref one, type_ref two) {
    return (find(one) == find(two));
}

//If the bound is a polytype, then if the bound is more general
//than in's bound, return 0 [failure]. If it's more specific, 
//restrict "in"'s bound to fall under bound
//If the bound is a type_ref, then unify the two type_refs.
//Returns 1 unless they are already unified
int type_ref_restrictbound(type_ref in, typeslot bound) {
    type_ref bound_ref = typeslot_get_ref(bound);
    if (bound_ref == NULL) {
        //Polytype case
        polytype bound_type = typeslot_get_type(bound);
        //If the bound type is too general
        if (!is_subtype(bound_type, type_ref_getbound(in))) {
            return 0;
        }
        //Otherwise, call type_ref_restrict
        return type_ref_restrict(in, bound_type);
    }
    //type_ref case
    int result = !type_refs_same_class(in, bound_ref);
    unify_type_refs(in, bound_ref);
    return result;
}


int type_ref_constrain(type_ref in, typeslot bound) {
    //Try to see if the incoming bound is actually parametric
    type_ref bound_ref = typeslot_get_ref(bound);
    if (bound_ref == NULL) {
        //Must not actually be a type_ref, so just call restrict
        return type_ref_restrict(in, typeslot_get_type(bound));
    }
    //Otherwise, we should unify the constraints of the two type_refs
    int active = !type_refs_same_class(in, bound_ref); //Active iff they weren't in the same class to begin with
    unify_type_refs(in, bound_ref);
    return active;
}

type_ref type_ref_addequations(type_ref in, type_equation_dynarray eqns) {
    int i;
    for (i=0; i < eqns.size; i++) {
        in = type_ref_add_equation(in, eqns.begin[i]);
    }
    return in;
}

//TODO: Make this so the equations between the two are concatenated
type_ref unify_type_refs(type_ref one, type_ref two) {
    //If they're already unified
    if (type_refs_same_class(one, two)) {
        return one;
    }
    polytype onebound = type_ref_getbound(one);
    polytype twobound = type_ref_getbound(two);
    polytype newbound = intersect_types(onebound, twobound);
    type_ref_setbound(two, newbound);
    type_ref_addequations(two, type_equation_dynarray_copy(type_ref_get_equations(one)));
    type_ref_makepoint(one, two);
    return one;
}


//Really dumb way to print type refs FIXME: Make this generate sensible unique names
void print_type_ref(type_ref in, nametable names) {
    printf(" [");
    int tmp = (int) find(in);
    char printchar = (tmp % 27) + 64; //Make it some dumb printable letter/character
    printf("%c <: ", printchar);
    print_type(type_ref_getbound(in), names);
    printf("] ");
    return;
}

void print_type_ref_list(type_ref_dynarray in, nametable names) {
    int i;
    for (i=0; i < in.size; i++) {
        print_type_ref(in.begin[i], names);
        printf(",");
    }
    return;
}

