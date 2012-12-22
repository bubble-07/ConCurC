#include <stdio.h>
#include <string.h>


#include "../lexer.h"
               
int main(int argc, const char * argv[]) {
    lex_result result = lex();
    int i = 0;
//    printf("%f", 40.0);
    while (i < result.program.size) {
        if (result.program.begin[i].tokenval == 4) {
            printf("%f", result.program.begin[i].attr.floatval);
        }
        if (result.program.begin[i].tokenval == 5) {
            printf("%d", result.program.begin[i].attr.intval);
        }
        else {
            printf("%d", (int)result.program.begin[i].tokenval);
        }
        printf("%s", "\n");
        i++;
    }
    return 0;
}
