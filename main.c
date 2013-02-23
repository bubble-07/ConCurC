#include "lexer.h"

#define TOK in.program.begin[i].tokenval
#define GENPRINT(tok) else if (in.program.begin[i].tokenval == tok) { printf("%s", #tok); }
void display(lex_result in) {
    int i;
    for (i = 0; i < in.program.size; i++) {
        if (TOK == INT) {
            printf("%s", "integer: ");
            printf("%d", in.program.begin[i].attr.intval);
        }
        else if (TOK == FLOAT) {
            printf("%s", "float: ");
            printf("%.20f", in.program.begin[i].attr.floatval);
        }
        else if (TOK == STRING) {
            printf("%s", "STRING CONSTANT");
        }
        GENPRINT(DEF)
        GENPRINT(LAMBDA)
        GENPRINT(NAMESPACE)
        GENPRINT(IMPORT)
        GENPRINT(TYPE)
        GENPRINT(SUBS)
        GENPRINT(SUPS) 
        else {
            printf("%s", "identifier: ");
            printf("%d", TOK);
        }
        printf("%s", "   ");
    }
    return;
}

int main(int argc, const char * argv[]) {
    lex_result lexresult;
    if (argc < 2) {
        lexresult = lex(stdin);
    }
    else {
        FILE * tolex = fopen(argv[1], "r");
        if (tolex == NULL) {
            printf("%s", "WTF?");
        }
        lexresult = lex(tolex);
        fclose(tolex);
    }
    display(lexresult);
    printf("%s", "\n");
    return 0;
}
