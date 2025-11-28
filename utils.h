#pragma once

#include <stdio.h>
#include <stdlib.h>

const size_t UTILS_WRITE_BUF = 1024*10;

#define panic(msg) {\
    fprintf(stderr, "PANIC: %s:%d %s \n", __FILE__, __LINE__, msg);\
    abort();\
}

#define assert(cmp, str){\
    if(!(cmp)){\
        char msg[UTILS_WRITE_BUF];\
        snprintf(msg, UTILS_WRITE_BUF,"Check for %s failed: %s",#cmp, (str));\
        panic(msg);\
    }\
}
