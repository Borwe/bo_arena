#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <errno.h>
#include "utils.h"

#include <bo_arena.h>

#define runTest(test) {\
    fprintf(stderr,"Running Test: %s\n",#test);\
    test();\
}

void cleanUpMmapArena(bo_arena *arena){
    const int r = munmap(arena->memory, arena->size);
    if(r==-1){
        const char * msg = strerror(errno);
        panic(msg);
    }
    arena->memory = NULL;
}

void testClearing(void){
    bo_arena arena  = bo_make_arena(
        mmap(NULL, 100,
             PROT_WRITE | PROT_WRITE,
             MAP_PRIVATE | MAP_ANONYMOUS, -1, 0),
        100, false, cleanUpMmapArena);
    arena.cleanup(&arena);
    assert(arena.memory == NULL, "Expected memory to be null after clearing");
}

void testUsing(void){
    bo_arena arena  = bo_make_arena(
        mmap(NULL, 100,
             PROT_WRITE | PROT_WRITE,
             MAP_PRIVATE | MAP_ANONYMOUS, -1, 0),
        100, false, cleanUpMmapArena);
    char *msg;
    bo_allocate_items(msg,true, &arena, char, 6);
    strcpy(msg, "Brian");
    assert(msg[1]=='r', "Unexpected");
    assert(msg[4]=='n', "Unexpected");
    arena.cleanup(&arena);
    assert(arena.memory == NULL, "Expected memory to be null after clearing");
}

void testOverUsing(void){
    bo_arena arena  = bo_make_arena(
        mmap(NULL, 100,
             PROT_WRITE | PROT_WRITE,
             MAP_PRIVATE | MAP_ANONYMOUS, -1, 0),
        100, false, cleanUpMmapArena);
    char *msg;
    bo_allocate_items(msg, false, &arena, char, 200);
    assert(msg==NULL, "Expected to fail allocation");
    arena.cleanup(&arena);
    assert(arena.memory == NULL, "Expected memory to be null after clearing");
}


void testOverUsingLater(void){
    bo_arena arena  = bo_make_arena(
        mmap(NULL, 100,
             PROT_WRITE | PROT_WRITE,
             MAP_PRIVATE | MAP_ANONYMOUS, -1, 0),
        100, false, cleanUpMmapArena);
    char *msg;
    bo_allocate_items(msg, true, &arena, char, 20);
    bo_allocate_items(msg, false, &arena, char, 81);
    assert(msg==NULL, "Expected to fail allocation");
    arena.cleanup(&arena);
    assert(arena.memory == NULL, "Expected memory to be null after clearing");
}


void testFreeing(void){
    bo_arena arena  = bo_make_arena(
        mmap(NULL, 100,
             PROT_WRITE | PROT_WRITE,
             MAP_PRIVATE | MAP_ANONYMOUS, -1, 0),
        100, true, cleanUpMmapArena);
    char *msg;
    bo_allocate_items(msg, true, &arena, char, 20);
    bo_allocate_items(msg, false, &arena, char, 81);
    assert(msg==NULL, "Expected to fail allocation");
    bo_arena_free(&arena,msg);
    assert(arena.head == NULL, "Expected to have nothing");

    char error_msg[5*1024];
    snprintf(error_msg, 5*1024,"Expected ptr == memory but got %p == %p", arena.ptr, arena.memory);
    assert(arena.ptr == arena.memory, error_msg);
    bo_allocate_items(msg, true, &arena, char, 81);
    arena.cleanup(&arena);
    assert(arena.memory == NULL, "Expected memory to be null after clearing");
}

int main(void){
    runTest(testClearing);
    runTest(testUsing);
    runTest(testOverUsing);
    runTest(testOverUsingLater);
    runTest(testFreeing);
    return 0;
}
