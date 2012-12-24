#include "dict.h"
#ifndef DYNSTRING_DEFINED
#include <string.h>
#include <stdlib.h>
#define DYNSTRING_DEFINED
//Definition of dynamic strings, and operations and setup for using strings in a dict
DEFINE_DYNARRAY(char)
typedef char_dynarray string;

/*Creates a dynstring from a traditional C string*/
inline static string to_dynstring(char* in) {
    int i = 0;
    string result = char_dynarray_make(1);
    while (in[i]) {
        result = char_dynarray_add(result, in[i]);
        i++;
    }
    return result;
}

/*Methods to allow them to be used in dictionaries*/

//FIXME: Better hash function, please!
inline static size_t hash_string(string in) {
    size_t result = 0;
    size_t i = 0;
    while (i < in.size) {
        result += in.begin[i];
        i++;
    }
    return result;
}

inline static int string_eq(string one, string two) {
    size_t i = 0;
    if (one.size != two.size) {
        return 0;
    }
    while (i < one.size) {
        if (one.begin[i] != two.begin[i]) {
            return 0;
        }
        i++;
    }
    return 1;
}
#endif
