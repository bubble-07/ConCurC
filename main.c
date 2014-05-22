#include "prims/type.h"
int main() {

    init_type_universe();

    TypeRef Int = get_TypeRef(to_dynstring("Int"));
    TypeRef Num = get_TypeRef(to_dynstring("Num"));
    TypeRef String = get_TypeRef(to_dynstring("String"));
    
    TypeInfo Both = add_type(add_type(make_empty_type(), String), Num);
    //TypeInfo Both = intersect_types(Num, Top);

    TypeInfo Restrictor = add_type(add_type(make_empty_type(), Top), Int);

    print_type(Both);

    Both = restrict_sum(Both, Restrictor);


    print_type(Both);

    return 0;
}
