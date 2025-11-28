#ifndef DEHASH_ARENA
#define DEHASH_ARENA

#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdalign.h>
#include <stdbool.h>

typedef struct bo_arena bo_arena; 

typedef void (*bo_arena_cleanup_func)(bo_arena *);

typedef struct bo_arena_alocation {
    struct bo_arena_alocation *back;
    uint64_t size;
    void *location;
    struct bo_arena_alocation  *next;
}bo_arena_alocation;

typedef struct bo_arena {
    void *memory;
    void *ptr;
    uint64_t size;
    bool freeing;
    bo_arena_cleanup_func cleanup;
    bo_arena_alocation *head;
}bo_arena;

/*
*/
bo_arena bo_make_arena(
    void *memory, uint64_t size,
    bool isFreeingKind, bo_arena_cleanup_func cleanupFunc){

    if(memory==NULL){
        panic("Passed in memory that is null");
    }
    return (bo_arena){
        .memory = memory,
        .ptr = memory,
        .freeing = isFreeingKind,
        .size = size,
        .cleanup = cleanupFunc
    };
}

void *allocate(bo_arena *area, uint64_t alignment, uint64_t size, uint64_t count){
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

void bo_arena_free(bo_arena *arena, void *item){
}

#define bo_allocate_items(ptr, panicc, arena, typ, count) {\
    const size_t alignment = alignof(typ); \
    typ *ptr_n = allocate((arena), alignment, sizeof(typ), (count));\
    if((panicc) && ptr_n==NULL){\
        panic("Failed to allocate using Arena");\
    }\
    (ptr) = ptr_n;\
}

#endif
