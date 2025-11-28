#pragma once

#include <stdio.h>
#include <stdlib.h>

#define panic(msg) {\
    fprintf(stderr, "PANIC: %s:%d %s \n", __FILE__, __LINE__, msg);\
    abort();\
}
