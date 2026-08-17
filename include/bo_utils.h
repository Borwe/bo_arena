#ifndef BO_ARENA_UTILS_H
#define BO_ARENA_UTILS_H

#include <stdio.h>
#include <stdlib.h>

#define UTILS_WRITE_BUF 1024*10

#define bo_arena_panic(msg) {\
    fprintf(stderr, "PANIC: %s:%d %s \n", __FILE__, __LINE__, msg);\
    exit(-1);\
}

void _bo_arena_assert(const char*const cmp_str, const char*const str){
        char msg[UTILS_WRITE_BUF];
        snprintf(msg, UTILS_WRITE_BUF,"Check for %s failed: %s",cmp_str, str);
        bo_arena_panic(msg);
}

#define bo_arena_assert(cmp, str){\
    if(!(cmp)){\
        _bo_arena_assert(#cmp, str);\
    }\
}

#endif
