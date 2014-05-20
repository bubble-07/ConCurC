#include "type.h"
int main() {

    init_type_universe();

    TypeRef Int = get_TypeRef(to_dynstring("Int"));
    TypeRef Num = get_TypeRef(to_dynstring("Num"));
    
    //TypeInfo Both = add_type(add_type(make_empty_type(), Int), Num);
    TypeInfo Both = intersect_types(Num, Top);

    print_type(Both);

    Both = simplify_TypeInfo(Both); //Need to fix this


    print_type(Both);

    return 0;
}
