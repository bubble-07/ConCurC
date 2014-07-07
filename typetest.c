#include "passes/typeinfer.h"
#include "passes/primorder.h"
#include "passes/collectnames.h"
#include "passes/to_cells.h"
#include "prims/polytype.h"
#include "prims/type_graph.h"

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
    finalize_type_universe();

    
    polytype Int = make_monotype(get_TypeGraphRef(INTID_LEXID));
    polytype String = make_monotype(get_TypeGraphRef(STRINGID_LEXID));

    typeslot_dynarray IntString = typeslot_dynarray_make(1);
    IntString = typeslot_dynarray_add(IntString, typeslot_from_type(Int));
    IntString = typeslot_dynarray_add(IntString, typeslot_from_type(String));

    polytype EitherIntString = make_polytype(Either, IntString);

    polytype result = intersect_types(Int, EitherIntString);

    print_type(result, program.parse.names); //Print out the result
    return 0;
}



