#include <stdio.h>
#include <string.h>


#include "../lexer.h"
               
int main(int argc, const char * argv[]) {
    FILE* readfrom = fopen("test.txt", "r");
    lex_result result = lex(readfrom);
    int i = 0;
//    printf("%f", 40.0);
    while (i < result.program.size) {
        if (result.program.begin[i].tokenval == 4) {
            printf("%s", "BEGIN");
        }
        if (result.program.begin[i].tokenval == 5) {
            printf("%s", "END");
        }
        else {
            printf("%d", (int)result.program.begin[i].tokenval);
        }
        printf("%s", "\n");
        i++;
    }
    return 0;
}
