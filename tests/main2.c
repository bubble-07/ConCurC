#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef char * charptr;

size_t hash_charptr(charptr in) {
    size_t result = 0;
    while (*in) {
        result += *in;
        in++;
    }
    return result;
}

#include "dict.h"
DEFINE_DICT(charptr, int)

int main(int argc, const char * argv[]) {
    charptr_int_dict test = charptr_int_dict_init(5);
    test = charptr_int_dict_add(charptr_int_dict_add(test, charptr_int_bucket_make("hi", 5)), charptr_int_bucket_make("siwwy", 10));
    printf("%d", charptr_int_dict_get(test, "hi"));
    return 0;
}
