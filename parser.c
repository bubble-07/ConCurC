#include "parser.h"
lexid display(lexid in, lexid_tree_dynarray other) {
    printf("%d", in.tokenval);
    return in;
}

int main(int argc, const char * argv[]) {
    parse_result parseresult = parse(lex());
    lexid_tree AST = parseresult.AST;
    lexid_tree_dfmap(AST, &display);    
    return 0;
}
                
                


