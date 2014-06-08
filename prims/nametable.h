/* Defines a name table, used for finding the name
[in the source] of a given lexid */

#include "../libs/dynstring.h"
#include "lexid.h"
#include "symboltable.h"

#ifndef NAMETABLE_DEFINED
#define NAMETABLE_DEFINED

//Internally store a nametable as an array of strings,
//which are to be indexed by token values
typedef string_dynarray nametable;

//Create a nametable from a table of symbols.
//Len represents the total number of lexids to be stored
static nametable nametable_make(string_lexid_dict symtable, size_t len) {
    nametable result = string_dynarray_make(len + 1);
    //Initially fill the table with empty entries
    result = string_dynarray_fill(result, empty_string, len);

    //Go through the whole dictionary element-by-element
    size_t rownum;
    size_t colnum;
    for (rownum=0; rownum < symtable.size; rownum++) {
        string_lexid_bucket_dynarray row = symtable.begin[rownum];
        for (colnum=0; colnum < row.size; colnum++) {
            size_t tokenval = row.begin[colnum].value.tokenval; //Get the token value
            result.begin[tokenval] = row.begin[colnum].key; //Assign the tokenval-th entry in the result to the key
        }
    }

    return result;
}
            
//Gets the name of a passed lexid
static string nametable_get(nametable in, lexid token) {
    //TODO: Add error handling!
    return in.begin[token.tokenval];
}
//Gets the name of a passed lexid as a c string
static char * nametable_get_cstring(nametable in, lexid token) {
    return to_cstring(nametable_get(in, token));
}

#endif
