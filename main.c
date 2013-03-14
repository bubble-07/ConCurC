#include "parser.h"
#include "primorder.h"

#define GENPRINT(tok) else if (in.data.tokenval == tok) { printf("%s", #tok); }
void display(lexid_tree in, string_dynarray backsymtable) {
    string toprint;
    printf("%s", "\n on line: ");
    printf("%d", (int) in.data.loc.lineno);
    printf("%s", " ");

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
    if (argc < 2) {
        parseresult = primorder(parse(lex(stdin)));
    }
    else {
        FILE * toparse = fopen(argv[1], "r");
        if (toparse == NULL) {
            printf("%s", "WTF?");
        }
        parseresult = primorder(parse(lex(toparse)));
        fclose(toparse);
    }
    lexid_tree AST = parseresult.AST;
    display(AST, parseresult.backsymtable);
    printf("%s", "\n");
    return 0;
}
