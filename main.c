#include "parser.h"
#include "primorder.h"

#define GENPRINT(tok) else if (in.data.tokenval == tok) { printf("%s", #tok); }
void display(lexid_tree in) {
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
            display(in.children.begin[i]);
            i++;
        }
        printf("%s", ")");
    }
    else {
        printf("%s", "identifier: ");
        printf("%d", in.data.tokenval);
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
    display(AST);
    printf("%s", "\n");
    return 0;
}
