# BO Arena
### A set of functions in C to manage memory using arenas in C, similar to Zig's

## Usage:

This is a two header file library, all files contained in [include](/include) directory, you can also use [meson wrapping](https://mesonbuild.com/Wrap-dependency-system-manual.html) to use the library, example, a file in `submodules/bo_arena.wrap` with the contents:

```meson
[wrap-git]
url = https://github.com/Borwe/bo_arena.git
revision = HEAD
depth = 1
```

To create an arena you call the function:

```c
bo_arena bo_make_arena(
    void *memory, uint64_t size,
    bool isFreeingKind, bo_arena_cleanup_func cleanupFunc)
```

- In the `memory` field you pass an array to be used as the memory.
- In the `isFreeingKind` you pass in true, if you intend be able to free and reuse the arena. Note this might cause extreme fragmentation of the buffer, 
- In the `cleanupFunc` it's a variable you pass in a function so you can hand deleting the memory of the arena fully, when you call `arena.cleanup(&arena);` the function signature is `void bo_arena_cleanup_func(bo_arena *arena)`

You can then allocate memory by calling the macro:
```c
bo_allocate_items(ptr, panicc, arena, typ, count)
```

- `ptr` is a pointer that you have declared, and set to null, that the allocator should setup.
- `panicc` is field you set to true, and it will make your application panic, if the arena doesn't have enough space, else if set to false it will just set the pointer you passed into it to NULL.
- `arena` The arena you created, you pass it in as a pointer.
- `typ` The type of the item you want to create, so arena can calculate layout.
- `count` The number of items of `typ` you want to create.

You can free space on the arena by calling:
```c
bo_arena_free(&arena,item);
```
- You pass in the pointer of item that you allocated on the arena,
- This only does something if you marked your arena as cleanable.

More examples are in the [testfile](./arena_test.c)


## Contributions
Contributions are open to this project.

## Testing
Project uses meson so you can just run
```sh
meson test -C build
```

and it will compile and run the tests
