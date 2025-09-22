#pragma once
#include "allocator_posix_align.h"

// 16 byte metadata
typedef struct  __attribute__((__packed__)) {
    uint64_t user_size;
    uint64_t user_alignment; // alignment in bytes
} malloc_metadata;


// [metadata][user-memory]

void * malloc_align(size_t size, posix_alignments alignment) {

    uint64_t real_size = sizeof(malloc_metadata) + size;                // reserve size for metadata
    real_size = calc_align_index_based(real_size - 1, alignment); // -1 because real_size is not an index

    void *real_block = NULL;
    const int ec = posix_memalign(&real_block, alignment, real_size);

    if (UNLIKELY(ec))
        return NULL;

    malloc_metadata *metadata = ((malloc_metadata *)real_block);
    metadata->user_size      = size;
    metadata->user_alignment = alignment;

#if DEBUG
    printf("address: %p\n", (void *)metadata);
    printf("user-address: %p\n", (void *)(((unsigned char *)real_block) + sizeof(malloc_metadata)));
#endif

    assert( (uintptr_t)(((unsigned char *)real_block) + sizeof(malloc_metadata)) % alignment == 0);

    return __builtin_assume_aligned(
            ((unsigned char *)real_block) + sizeof(malloc_metadata),
            alignment
    );
}


void * realloc_align(void *p, size_t size) {

    malloc_metadata *real_block = (malloc_metadata *) (((unsigned char *)p) - (
        sizeof(malloc_metadata)
    ));

#if DEBUG
    printf("user-address: %p\n", p);
    printf("calc address: %p\n", (void *)real_block);
#endif

    if (UNLIKELY(size == real_block->user_size))
        return p;

    void *user_memory = malloc_align(size, (posix_alignments)real_block->user_alignment);
    if (UNLIKELY(!user_memory))
        return NULL; // leave the block untouched


    // copy user-data into user_memory
    memcpy(
        __builtin_assume_aligned(user_memory, real_block->user_alignment),
        __builtin_assume_aligned(p, real_block->user_alignment),
        real_block->user_size
    );

    free(real_block);
    return user_memory;
}

void malloc_align_free(void *p) {

    malloc_metadata *real_block = (malloc_metadata *) (((unsigned char *)p) - (
            sizeof(malloc_metadata)
    ));

#if DEBUG
    printf("user-address: %p\n", p);
    printf("calc address: %p\n", (void *)real_block);
#endif

    free(real_block);
}