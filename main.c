#include "parser.h"
lexid display(lexid in, lexid_tree_dynarray other) {
    if (in.tokenval == INT) {
        printf("%s", "integer: ");
        printf("%d", in.attr.intval);
    }
    else if (in.tokenval == FLOAT) {
        printf("%s", "float: ");
        printf("%.20f", in.attr.floatval);
    }
    else {
        printf("%s", "identifier: ");
        printf("%d", in.tokenval);
    }
    printf("%s", "\n");
    return in;
}

int main(int argc, const char * argv[]) {
    parse_result parseresult = parse(lex());
    lexid_tree AST = parseresult.AST;
    lexid_tree_dfmap(AST, &display);    
    return 0;
}
                
                


