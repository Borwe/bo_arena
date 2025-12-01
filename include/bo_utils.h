#ifndef BO_ARENA_UTILS_H
#define BO_ARENA_UTILS_H

#include <stdio.h>
#include <stdlib.h>

const size_t UTILS_WRITE_BUF = 1024*10;

#define bo_arena_panic(msg) {\
    fprintf(stderr, "PANIC: %s:%d %s \n", __FILE__, __LINE__, msg);\
    abort();\
}

#define bo_arena_assert(cmp, str){\
    if(!(cmp)){\
        char msg[UTILS_WRITE_BUF];\
        snprintf(msg, UTILS_WRITE_BUF,"Check for %s failed: %s",#cmp, (str));\
        bo_arena_panic(msg);\
    }\
}

#endif
