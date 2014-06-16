#include "../libs/memoryman.h"
#include "type_ref.h"

type_ref make_empty_type_ref() {
    type_ref result = memalloc(sizeof(type_ref_node));
    result->forward = NULL; //Not forwarding
    result->type = make_empty_type(); //Stores nothing
    result->kind = type_ref_empty; //Set the kind to empty
    return result;
}

type_ref make_known_type_ref(TypeInfo in) {
    type_ref result = make_empty_type_ref();
    result->type = in;
    result->kind = type_ref_immediate;
    return result;
}
type_ref copy_type_ref_data(type_ref target, type_ref source) {
    target->kind = source->kind;
    target->type = source->type;
    target->forward = source->forward;
    return target;
}

//TODO: Handle forwarding nodes here!
type_ref concat_type_refs(type_ref in, type_ref a) {
    if (in->kind == type_ref_empty) {
        //Just copy along the stuff in a
        return copy_type_ref_data(in, a);
    }
    else if (in->kind == type_ref_immediate) {
        //We have some kind of immediate type
        if (a->kind == type_ref_empty) {
            //But if the other one is empty, do nothing
            return in;
        }
        else if (a->kind == type_ref_immediate) {
            //Yay, get to actually do something -- sum the two types
            in->type = concat_types(in->type, a->type);
            return in;
        }
    }
    return in; //Usable default for now
}

//Really dumb way to print type refs FIXME: Make this generate sensible unique names
void print_type_ref(type_ref in, nametable names) {
    if (in->kind == type_ref_immediate) {
        //Actually print out the type it has
        print_type(in->type, names);
    }
    else {
        printf(" %p ", in); //Otherwise, just print the pointer. Woo.
    }
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

