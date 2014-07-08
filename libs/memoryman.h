#include <stdlib.h>
#include <string.h>
#ifndef MEMMAN
#include <stdio.h>

#define MEMMAN
/*Defines convenience functions for allocating and dealing with memory*/

inline static void outofmemexception() {
     printf("%s", "you did something stupid -- OUT OF MEMORY!");
}

/*wraps malloc*/
inline static void* memalloc(int size) {
    void* result = malloc(size);
    if (result == NULL) {
        outofmemexception();
    }
    return result;
}

/*wraps realloc*/
inline static void* memrealloc(void* ptr, size_t size) {
    void * result = realloc(ptr, size);
    if (result == NULL) {
        outofmemexception();
    }
    return result;
}

#endif
