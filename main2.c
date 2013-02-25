#include "parser.h"
#include "primorder.h"

#define GENPRINT(tok) else if (in.data.tokenval == tok) { printf("%s", #tok); }
void display(lexid_dynarray in) {
    int i;
    for (i=0; i < in.size; i++) {
        printf("%s", "\n token: ");
        printf("%d", in.begin[i].tokenval);
    }
    printf("%s", "   ");
    return;
}

int main(int argc, const char * argv[]) {
    lex_result parseresult;
    if (argc < 2) {
        parseresult = lex(stdin);
    }
    else {
        FILE * toparse = fopen(argv[1], "r");
        if (toparse == NULL) {
            printf("%s", "WTF?");
        }
        parseresult = lex(toparse);
        fclose(toparse);
    }
    display(parseresult.program);
    printf("%s", "\n");
    return 0;
}
