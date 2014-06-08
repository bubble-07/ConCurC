#include "passes/primorder.h"
#include "passes/collectnames.h"
#include "passes/to_cells.h"
#include "passes/typeinfer.h"

//Being used to test function name collection on single file

extern char* realpath(const char* path, char* resolved_path);

#define GENPRINT(tok) else if (in.data.tokenval == tok) { printf("%s", #tok); }
void display(lexid_tree in, string_dynarray backsymtable) {
    string toprint;
    printf("%s", "\n on line: ");
    printf("%d", (int) in.data.loc.lineno);
    printf("%s", " col: ");
    printf("%d", (int) in.data.loc.linepos);
    printf(" ");
    
    if (in.data.tokenval == INT) {
        printf("%s", "integer: ");
        printf("%d", in.data.attr.intval);
    }
    else if (in.data.tokenval == FLOAT) {
        printf("%s", "float: ");
        printf("%.20f", in.data.attr.floatval);
    }
    else if (in.data.tokenval == STRING) {
        printf("%s", "STRING CONSTANT");
    }
    else if (in.data.tokenval == FILEREF) {
        printf("FILE REF: %s", to_cstring(in.data.attr.stringval));
    }
    GENPRINT(DEF)
    GENPRINT(LAMBDA)
    GENPRINT(NAMESPACE)
    GENPRINT(IMPORT)
    GENPRINT(TYPE)
    GENPRINT(SUBS)
    GENPRINT(SUPS)
    else if (in.data.tokenval == EXPR) {
        printf("%s", "(");
        size_t i = 0;
        while (i < in.children.size) {
            display(in.children.begin[i], backsymtable);
            i++;
        }
        printf("%s", ")");
    }
    else {
        toprint = char_dynarray_copy(backsymtable.begin[in.data.tokenval]);
        toprint = char_dynarray_add(toprint, (char) 0);
        printf("%s", toprint.begin);
    }
    printf("%s", "   ");
    return;
}

int main(int argc, const char* argv[]) {
    fileLoc* file;
    if (argc < 2) {
        file = load_stdin();
    }
    else {
        file = load_file(realpath(argv[1], NULL));
    }
    collectnames_result program = collectnames(primorder(parse(lex(file))));

    //Initialize the type universe so we can do things.
    init_type_universe();
    
    //Print out our parse tree
    display(program.parse.AST, program.parse.backsymtable);

    //Convert to a list of definitions
    def_collection defs = to_cells(program);

    printf("\n\n Inferring types \n\n");

    defs = typeinfer(defs);
    
    return 0;
}
