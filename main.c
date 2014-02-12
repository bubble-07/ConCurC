#include "libs/dynstring.h"
#include "libs/digraph.h"

typedef string Type;

DEFINE_GRAPH(Type)

Type_graph Universe;

#define AddType(name) noderef name; Universe = Type_graph_addnode(Universe, to_dynstring(#name), &name);
#define Subs(one, two) Universe = Type_graph_addedge(Universe, two, one);

//TODO: Write method to print the current type graph

typedef struct {
    noderef input;
    noderef output;
} Function;

void Type_graph_print(Type_graph in) {
    size_t i, j;
    for (i = 0; i < in.size; i++) {
        for (j = 0; j < in.size; j++) {
            if (Type_graph_testedge(in, i, j)) {
                printf("%s <: %s \n", to_cstring(Type_graph_getnode(in, j)), to_cstring(Type_graph_getnode(in, i)));
            }
        }
    }
    return;
}

Function make_function(noderef in, noderef out) {
    Function result;
    result.input = in;
    result.output = out;
    return result;
}

DEFINE_DYNARRAY(Function)

Function_dynarray FunctionChain;

int typecheck() {
    size_t i;
    for (i = 0; i < FunctionChain.size - 1; i++) {
        if (!Type_graph_testedge(Universe, FunctionChain.begin[i + 1].input, FunctionChain.begin[i].output)) {
            return 0;
        }
    }
    return 1;
}


int main() {
    Universe = Type_graph_init(2);
    AddType(Any)
    AddType(String)
    AddType(Number)
    AddType(Int)
    Subs(String, Any)
    Subs(Number, Any)
    Subs(Int, Number)
    Universe = Type_graph_transitiveclosure(Universe);
    Universe = Type_graph_reflexiveclosure(Universe);
    Type_graph_print(Universe);

    FunctionChain = Function_dynarray_make(1);
    FunctionChain = Function_dynarray_add(FunctionChain, make_function(String, Any));
    FunctionChain = Function_dynarray_add(FunctionChain, make_function(Any, Number));
    FunctionChain = Function_dynarray_add(FunctionChain, make_function(Int, Int));

    if (typecheck()) {
        printf("All well \n");
    }


    return 0;
}
    


