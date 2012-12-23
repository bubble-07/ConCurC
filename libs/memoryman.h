#ifndef MEMMAN
#include <stdio.h>
#define MEMMAN
inline static void outofmemexception() {
     printf("%s", "you did something stupid");
     while (1 == 1) { }
}


inline static void* memalloc(int size) {
    void* result = malloc(size);
    if (result == NULL) {
        outofmemexception();
    }
    return result;
}
inline static void* memrealloc(void* ptr, size_t size) {
    void * result = realloc(ptr, size);
    if (result == NULL) {
        outofmemexception();
    }
    return result;
}

#endif
