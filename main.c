#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dynarray.h"
#include "memoryman.h"

DEFINE_DYNARRAY(char)

char doubleit(char in) {
    return in * 2;
}
    

int main (int argc, const char * argv[]) {
    char_dynarray test = char_dynarray_fromarray((char[]){1, 2, 3, 4, 5, 6}, 6);
    char_dynarray second = char_dynarray_copy(test);
    test = char_dynarray_cat(test, second);
    test = char_dynarray_map(test, &doubleit); 
    free(second.begin);
    return test.begin[5];
}
