#include "passes/typeinfer.h"
#include "passes/primorder.h"
#include "passes/collectnames.h"
#include "passes/to_cells.h"

int main(int argc, const char* argv[]) {    
    
    fileLoc* file;

    if (argc < 2) {
        file = load_stdin();
    }
    else {
        file = load_file(realpath(argv[1], NULL));
    }
    collectnames_result program = collectnames(primorder(parse(lex(file))));


    init_type_universe();

    
    polytype Int = make_monotype(get_TypeGraphRef(INTID_LEXID));
    polytype String = make_monotype(get_TypeGraphRef(STRINGID_LEXID));

    type_ref_dynarray IntString = type_ref_dynarray_make(1);
    IntString = type_ref_dynarray_add(IntString, make_known_type_ref(make_simple_type(Int)));
    IntString = type_ref_dynarray_add(IntString, make_known_type_ref(make_simple_type(String)));

    polytype EitherIntString = make_polytype(get_TypeGraphRef(EITHERID_LEXID), IntString);

    TypeInfo result = intersect_types(Int, EitherIntString);

    print_type(result, program.parse.names); //Print out the result
    return 0;
}



