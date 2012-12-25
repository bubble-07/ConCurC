#include "parser.h"
#include "primorder.h"

#define GENPRINT(tok) else if (in.tokenval == tok) { printf("%s", #tok); }
lexid display(lexid in, lexid_tree_dynarray other) {
    if (in.tokenval == INT) {
        printf("%s", "integer: ");
        printf("%d", in.attr.intval);
    }
    else if (in.tokenval == FLOAT) {
        printf("%s", "float: ");
        printf("%.20f", in.attr.floatval);
    }
    else if (in.tokenval == STRING) {
        printf("%s", "STRING CONSTANT");
    }
    GENPRINT(DEF)
    GENPRINT(LAMBDA)
    GENPRINT(NAMESPACE)
    GENPRINT(IMPORT)
    GENPRINT(TYPE)
    GENPRINT(SUBS)
    GENPRINT(SUPS)
    GENPRINT(EXPR)
    else {
        printf("%s", "identifier: ");
        printf("%d", in.tokenval);
    }
    printf("%s", "\n");
    return in;
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
    lexid_tree_dfmap(AST, &display);    
    return 0;
}
                
                


