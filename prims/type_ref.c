#include "../libs/memoryman.h"
#include "type_ref.h"
#include "type_ref_info.h"

type_ref make_known_type_ref(TypeInfo in) {
    type_ref_info* info = memalloc(sizeof(type_ref_info));
    info->upperbound = in;
    info->equations = type_equation_dynarray_make(1);
    type_ref result = memalloc(sizeof(type_ref_node));
    result->data = (void*) info;
    result->kind = type_ref_representative; //This is the representative node
    return result;
}

type_ref make_unknown_type_ref() {
    return make_known_type_ref(make_unknown_type());
}

//Gets the representative node
type_ref find(type_ref in) {
    if (in->kind == type_ref_representative) {
        return in;
    }
    //Otherwise, follow the chain
    return find((type_ref) in->data);
}

int hash_type_ref(type_ref in) {
    return (int) find(in); //Return address of representative
}

int type_ref_eq(type_ref one, type_ref two) {
    return find(one) == find(two); //Equal iff representative addresses equal
}

//Gets the principal bounding type of the given typeref
TypeInfo type_ref_getbound(type_ref in) {
    type_ref rep = find(in);
    type_ref_info* info = rep->data;
    return info->upperbound;
}

int type_ref_restrict(type_ref in, TypeInfo bound) {
    type_ref rep = find(in);
    type_ref_info* info = rep->data;

    TypeInfo oldbound = copy_type(info->upperbound);

    info->upperbound = restrict_sum(info->upperbound, bound);
    return !type_eq(oldbound, info->upperbound); //Active if the upper bound has changed
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

