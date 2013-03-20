#include "parser.h"
#include "primorder.h"
#include "det_depends.h"

#define GENPRINT(tok) else if (in.data.tokenval == tok) { printf("%s", #tok); }
void display(lexid_tree in, string_dynarray backsymtable) {
    string toprint;
    printf("%s", "\n on line: ");
    printf("%d", (int) in.data.loc.lineno);
    printf("%s", " col: ");
    printf("%d", (int) in.data.loc.linepos);
    printf("%s", " ");
    printf("%s", " in file: ");
    printf("%s", to_cstring(in.data.loc.file));

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
        printf("%s", "FILE REF");
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

int main(int argc, const char * argv[]) {
    parse_result parseresult;
    fileLoc* file;
    if (argc < 2) {
        file = load_stdin();
        parseresult = deps_test(primorder(parse(lex(load_stdin()))));
    }
    else {
        file = load_file(argv[1]);
        parseresult = deps_test(primorder(parse(lex(file))));
    }
    close_file(file);
    free(file);
    lexid_tree AST = parseresult.AST;
    display(AST, parseresult.backsymtable);
    printf("%s", "\n");
    return 0;
}
