#include "dict.h"
#ifndef DYNSTRING_DEFINED
#include <string.h>
#include <stdlib.h>
#define DYNSTRING_DEFINED
//Definition of dynamic strings, and operations and setup for using strings in a dict
DEFINE_DYNARRAY(char)
typedef char_dynarray string;

/*Creates a dynstring from a traditional C string*/
inline static string to_dynstring(const char* in) {
    int i = 0;
    string result = char_dynarray_make(1);
    while (in[i] != '\0') {
        result = char_dynarray_add(result, in[i]);
        i++;
    }
    return result;
}
inline static char* to_cstring(string in) {
    char* result = (char*) malloc(sizeof(char) * (in.size + 1));
    int i;
    for (i=0; i < in.size; i++) {
        result[i] = in.begin[i];
    }
    result[i] = '\0';
    return result;
}

/*Methods to allow them to be used in dictionaries*/

//FIXME: Better hash function, please!
//NOTE: hashes are CASE-INSENSITIVE
inline static size_t hash_string(string in) {
    size_t result = 0;
    size_t i = 0;
    while (i < in.size) {
        result += tolower(in.begin[i]);
        i++;
    }
    return result;
}

//NOTE: String equality is CASE-INSENSITIVE on purpose.
inline static int string_eq(string one, string two) {
    size_t i = 0;
    if (one.size != two.size) {
        return 0;
    }
    if (one.size == 0 && two.size == 0) {
        return 1; //two empty strings are equal
        //(here to handle the null string)
    }
    while (i < one.size) {
        //Compare both characters converted to lowercase
        if (tolower(one.begin[i]) != tolower(two.begin[i])) {
            return 0;
        }
        i++;
    }
    return 1;
}

static string string_lookup_failure = {0,0,NULL}; //Define as empty, unallocated dynarray

DEFINE_DYNARRAY(string)

#endif
