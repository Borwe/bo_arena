#ifndef DEHASH_ARENA
#define DEHASH_ARENA

#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdalign.h>
#include <stdbool.h>

typedef struct Arena Arena; 

typedef void (*arenaCleanup)(Arena *);

typedef struct ArenaAlocation {
    struct ArenaAlocation *back;
    uint64_t size;
    void *location;
    struct ArenaAlocation *next;
}ArenaAlocation;

typedef struct Arena {
    void *memory;
    void *ptr;
    uint64_t size;
    bool freeing;
    arenaCleanup cleanup;
    ArenaAlocation *head;
}Arena;

Arena makeArena(void *memory, uint64_t size, bool isFreeingKind, arenaCleanup cleanupFunc){
    if(memory==NULL){
        panic("Passed in memory that is null");
    }
    return (Arena){
        .memory = memory,
        .ptr = memory,
        .freeing = isFreeingKind,
        .size = size,
        .cleanup = cleanupFunc
    };
}

void *allocate(Arena *area, uint64_t alignment, uint64_t size, uint64_t count){
    uintptr_t ptr = (uintptr_t)area->ptr;
    const uintptr_t ptr_end = ptr+(size*count);
    if((ptr & (alignment-1)) == 0 &&
        (ptr_end < (uintptr_t)(area->memory)+ area->size)){
        area->ptr = (void *)ptr_end;
        return area->ptr;
    }

    //get next aligned position
    ptr = (ptr+alignment-1) & ~(alignment -1);
    area->ptr = (void *)ptr_end;
    uintptr_t end = ((uintptr_t)area->memory) + area->size;
    if((uintptr_t)area->ptr > end){
        return NULL;
    }
    return (void*)ptr;
}

void arenaFree(Arena *arena, void *item){
}

#define allocate_items(ptr, panicc, arena, typ, count) {\
    const size_t alignment = alignof(typ); \
    typ *ptr_n = allocate((arena), alignment, sizeof(typ), (count));\
    if((panicc) && ptr_n==NULL){\
        panic("Failed to allocate using Arena");\
    }\
    (ptr) = ptr_n;\
}

#endif
