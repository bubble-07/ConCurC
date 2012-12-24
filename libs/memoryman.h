#ifndef MEMMAN
#include <stdio.h>
#define MEMMAN
/*Defines convenience functions for allocating and dealing with memory*/

inline static void outofmemexception() {
     printf("%s", "you did something stupid");
     while (1 == 1) { }
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
