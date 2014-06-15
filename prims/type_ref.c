#include "../libs/memoryman.h"
#include "type_ref.h"

type_ref make_empty_type_ref() {
    type_ref result = memalloc(sizeof(type_ref_node));
    result->forward = NULL; //Not forwarding
    //result->type = make_empty_type(); //Stores nothing
    result->kind = type_ref_empty; //Set the kind to empty
    return result;
}
