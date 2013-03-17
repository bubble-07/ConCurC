#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include "../libs/dynstring.h"

DEFINE_DYNARRAY(string)

int main(int argc, const char* argv[]) {
    char* truepath = realpath(argv[1], NULL);
    string_dynarray result = string_dynarray_make(1);
    string current = char_dynarray_make(1);
    string total = char_dynarray_make(1);
    int i;
    for (i=0; truepath[i] != '\0'; i++) {
        if (truepath[i] != '/') {
            current = char_dynarray_add(current, truepath[i]);
        }
        else {
            printf("%s", to_cstring(current));
            printf("%s", "     ");
            printf("%s", to_cstring(total));
            printf("%s", "\n");
            result = string_dynarray_add(result, current);
            current = char_dynarray_make(1);
        }
        total = char_dynarray_add(total, truepath[i]);
    }
    return 0;
}

