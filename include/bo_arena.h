#ifndef BO_ARENA
#define BO_ARENA

/** Make functions static or not*/
#ifdef BO_ARENA_STATIC
#define BO_ARENA_STATIC_ static
#else
#define BO_ARENA_STATIC_ 
#endif

#include "bo_utils.h"
#include <stdlib.h>
#include <stdint.h>
#include <stdalign.h>
#include <stdbool.h>

typedef struct bo_arena bo_arena; 
typedef struct bo_arena_alocation bo_arena_alocation;
typedef void (*bo_arena_cleanup_func)(bo_arena *);

struct bo_arena_alocation {
    struct bo_arena_alocation *back;
    uint64_t size;
    struct bo_arena_alocation  *next;
};

struct bo_arena {
    void *memory;
    void *ptr;
    uint64_t size;
    bool freeing;
    bo_arena_cleanup_func cleanup;
    bo_arena_alocation *head;
};

/*
 * Get a bo_arena, if you want the arena to be able
 * to reuse memory by cleaning using `bo_arena_free()`
 * then make sure you set @isFreeingKind to true,
 * else set it to false for noncleaning bo_arena
*/
BO_ARENA_STATIC_ bo_arena bo_make_arena(
    void *const memory, uint64_t size,
    bool isFreeingKind, bo_arena_cleanup_func cleanupFunc);

static void *bo_arena__allocate(
    bo_arena *area,
    const uint64_t alignment, const uint64_t size,const uint64_t count);

static bo_arena_alocation *bo_arena__make_allocation(bo_arena *const arena);

static void *bo_arena__update_allocation(
    bo_arena *restrict const arena,
    bo_arena_alocation **restrict aloc,
    void *restrict const location,
    uint64_t total_size);

static void *bo_arena__try_get_free_space(
    bo_arena *const arena,
    uint64_t alignment,
    uint64_t size, uint64_t count);


void bo_arena_free(bo_arena *const arena, void *const item){
    if(!arena->freeing){
        return;
    }
    if(arena->head == NULL){
        return;
    }
    static const uint64_t bo_arena_alocation_size = sizeof(bo_arena_alocation);
    bo_arena_alocation *aloc = arena->head;
    while(aloc->next !=NULL && (uintptr_t)aloc != (uintptr_t)item - bo_arena_alocation_size){
        aloc = aloc->next;
    }
    bo_arena_alocation *prev = aloc->back;
    if(prev != NULL){
        //if tree not empty there must always be a prev
        prev->next = aloc->next;
    }else{
        //reset memory since nothing in it
        arena->ptr = arena->memory;
        arena->head = NULL;
    }
}

#define bo_allocate_items(ptr, panicc, arena, typ, count) {\
    const size_t alignment = alignof(typ); \
    typ *ptr_n = bo_arena__allocate((arena), alignment, sizeof(typ), (count));\
    if((panicc) && ptr_n==NULL){\
        bo_arena_panic("Failed to allocate using Arena, ran out of space.");\
    }\
    (ptr) = ptr_n;\
}

/** To enable the functions of bo_arena file */
//#define BO_ARENA_IMPLENTATION 
#ifdef BO_ARENA_IMPLENTATION

static void *bo_arena__try_get_free_space(
    bo_arena *const arena,
    uint64_t alignment,
    uint64_t size, uint64_t count){

    static const uint64_t bo_arena_alocation_alignment = alignof(bo_arena_alocation);
    static const uint64_t bo_arena_size = sizeof(bo_arena_alocation);
    uint64_t total_size = (size * count) + bo_arena_size;
    uint64_t chosen_alignment = alignment>bo_arena_alocation_alignment? alignment :bo_arena_alocation_alignment;

    bo_arena_alocation *result = NULL;

    if(arena->head == NULL){
        return NULL;
    }

    bo_arena_alocation *aloc = arena->head;
    //head must never be null
    while(aloc->next!=NULL) aloc = aloc->next;

    uintptr_t ptr_loc = (uintptr_t)aloc + aloc->size;
    uintptr_t ptr_end = ptr_loc+total_size;
    const uintptr_t arena_end = (uintptr_t)arena->memory +arena->size;
    //if aligned
    if((ptr_loc & (chosen_alignment-1)) == 0
        && ptr_end < arena_end){
        result = (void*)ptr_loc;
        result->size = total_size;
    }else{
        //go to next aligned position
        ptr_loc = (ptr_loc+chosen_alignment-1) & ~(chosen_alignment -1);
        ptr_end = ptr_loc+total_size;
        if(ptr_end < arena_end){
            result = (void*)ptr_loc;
            result->size = total_size;
        }
    }

    if(result==NULL){
        return NULL;
    }else{
        aloc->next = result;
        result->back = aloc;
        void *ptr = (void *)((uintptr_t)result + result->size);
        return ptr;
    }
}

static void *bo_arena__update_allocation(
    bo_arena *restrict const arena,
    bo_arena_alocation **restrict aloc,
    void *restrict const location,
    uint64_t total_size){

    //since this function get's called even when aloc hasn't been created
    //always check and refuse to do use it
    if(arena->freeing){
        static const uint64_t bo_arena_alocation_size = sizeof(bo_arena_alocation);
        *aloc = location;
        (*aloc)->size = total_size;
        //set linkedlist pointers to null
        (*aloc)->next = NULL;
        (*aloc)->back = NULL;

        //add it to head linkedlist
        if(arena->head == NULL) {
            arena->head = *aloc;
            (*aloc)->back = NULL;
        }else{
            bo_arena_alocation *alptr = arena->head;
            while(alptr->next != NULL) { 
                alptr = alptr->next;
            }
            alptr->next = *aloc;
            (*aloc)->back = alptr;
        }

        return (void*)((uintptr_t)location + bo_arena_alocation_size);
    }else{
        return location;
    }
}

static void *bo_arena__allocate(
    bo_arena *const arena,
    const uint64_t alignment, const uint64_t size, const uint64_t count){

    bo_arena_alocation *aloc = NULL;
    static const uint64_t bo_arena_alocation_alignment = alignof(bo_arena_alocation);
    static const uint64_t bo_arena_size = sizeof(bo_arena_alocation);
    uint64_t total_size = size * count;
    uint64_t chosen_alignment = alignment;
    if(arena->freeing){
        if(alignment < bo_arena_alocation_alignment){
            chosen_alignment = bo_arena_alocation_alignment;
        }
        total_size = (size*count) + bo_arena_size;
    }

    const uintptr_t memory_end = ((uintptr_t)arena->memory) + arena->size;

    uintptr_t ptr = (uintptr_t)arena->ptr;
    uintptr_t ptr_end = ptr+total_size;
    if((ptr & (chosen_alignment-1)) == 0 && ptr_end < memory_end){
        arena->ptr = (void *)ptr_end;
        return bo_arena__update_allocation(arena, &aloc, (void*)ptr, total_size);
    }

    //get next aligned position
    ptr = (ptr+chosen_alignment-1) & ~(chosen_alignment -1);
    ptr_end = ptr+total_size;
    arena->ptr = (void*)ptr_end;
    if( ptr_end > memory_end){
        if(arena->freeing){
            return bo_arena__try_get_free_space(arena, alignment,size,count);
        }else{
            return NULL;
        }
    }
    return bo_arena__update_allocation(arena, &aloc, (void *)ptr, total_size);
}

static bo_arena_alocation *bo_arena__make_allocation(bo_arena *const arena){
    static const uint64_t alignment = alignof(bo_arena_alocation);
    static const uint64_t size = sizeof(bo_arena_alocation);
    bo_arena_alocation *allocation = bo_arena__allocate(arena, alignment, size, 1);

    if(allocation==NULL){
        return NULL;
    }else if(arena->head == NULL){
        arena->head = allocation;
        return arena->head;
    }else{
        //add it to end of linkedlist
        bo_arena_alocation *aloc = arena->head;
        while(aloc->next != NULL) aloc = aloc->next;
        aloc->next = allocation;
        return aloc->next;
    }
}

BO_ARENA_STATIC_ bo_arena bo_make_arena(
    void *const memory, uint64_t size,
    bool isFreeingKind, bo_arena_cleanup_func cleanupFunc){

    if(memory==NULL){
        bo_arena_panic("Passed in memory that is null");
    }
    return (bo_arena){
        .head = NULL,
        .memory = memory,
        .ptr = memory,
        .freeing = isFreeingKind,
        .size = size,
        .cleanup = cleanupFunc
    };
}

#endif

#endif
