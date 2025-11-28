#ifndef DEHASH_ARENA
#define DEHASH_ARENA

#include <stdint.h>
#include <stdalign.h>

typedef struct Arena {
    void *memory;
    void *ptr;
    uint64_t size;
}Arena;

Arena makeArena(void *memory, uint64_t size){
    return (Arena){
        .memory = memory,
        .ptr = memory,
        .size = size,
    };
}

void *allocate(Arena *area, uint64_t alignment, uint64_t size, uint64_t count){
    uintptr_t ptr = (uintptr_t)area->ptr;
    if((ptr & (alignment-1)) == 0){
        return area->ptr;
    }
    ptr = (ptr+alignment-1) & ~(alignment -1);
    area->ptr = (void *)(ptr+(size*count));
    if((uintptr_t)area->ptr > (uintptr_t)area->memory + size){
        return NULL;
    }
    return (void*)ptr;
}

#define allocate_items(ptr, arena, typ, count) (\
    const usize alignment = alignof(type); \
    ptr = allocate(arena, alignment, sizeof(*ptr), count);\
)

#endif
