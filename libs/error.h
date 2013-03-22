#include <stdlib.h>

#ifndef ERROR_DEFINED
#define ERROR_DEFINED
inline static void fatal_error(const char* message) {
    printf("FATAL ERROR! %s \n", message);
    exit(EXIT_FAILURE);
    return;
}
#endif
