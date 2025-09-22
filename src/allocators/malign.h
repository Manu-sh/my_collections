#pragma once
#include "allocator_posix_align.h"

#include <stdint.h>

// malign: memory aligned


// TODO: per garantire l'allineamento devo allineare il blocco dei metadati stesso però a quel punto diviene un problema risolvere se si usa una dimensione variabile
//  siccome l'allineamento richiesto da posix_malloc è che sia allineato a un multiplo della word posso ficcare un ptr davanti all'user pointer

// 16 byte metadata
typedef struct  __attribute__((__packed__)) {
    uint64_t user_size;
    uint64_t user_alignment; // alignment in bytes
} malloc_metadata;


static FORCED(inline) malloc_metadata * malign_internal_meta_get(void *user_pointer) {
    return (malloc_metadata *) (((uint8_t *)user_pointer) - (
            sizeof(malloc_metadata)
    ));
}

// la dimensione riservata al superblocco dipende dal tipo di allineamento richiesto dall'utente
static FORCED(inline) uint64_t dynamic_sizeof_meta(uint64_t user_alignment) {
    return calc_align_size_based(sizeof(malloc_metadata), user_alignment);
}


void * malign_internal_get_user_pointer(void *real_block) {

    malloc_metadata *metadata = (malloc_metadata *)real_block;

    // per garantire che l'user-pointer sia allineato anche il blocco dei metadati deve essere grande
    // almeno quanto l'allineamento richiesto dall'utente es. 64 byte per avx512

    uint64_t to_skip = dynamic_sizeof_meta(metadata->user_alignment);
    return __builtin_assume_aligned( // i hope flto help here cause user_alignment is a variable
        ((uint8_t *)real_block) + to_skip,
        metadata->user_alignment
    );
}


// [metadata][user-memory]

void * malign_alloc(size_t size, posix_alignments alignment) {

    //uint64_t real_size = sizeof(malloc_metadata) + size; // reserve size for metadata
    uint64_t real_size = dynamic_sizeof_meta(alignment) + size; // reserve size for metadata
    real_size = calc_align_size_based(real_size, alignment);    // -1 because real_size is not an index and doesn't require to be accessible

    void *real_block = NULL;
    const int ec = posix_memalign(&real_block, alignment, real_size);

    if (UNLIKELY(ec))
        return NULL;

    // setup metadata
    malloc_metadata *metadata = ((malloc_metadata *)real_block);
    metadata->user_size      = size;
    metadata->user_alignment = alignment;

    void *const user_pointer = malign_internal_get_user_pointer(real_block);

#if DEBUG
    printf("address: %p\n", (void *)metadata);
    printf("user-address: %p\n", user_pointer);
#endif

    assert( ((uintptr_t)((uint8_t *)user_pointer)) % alignment == 0);

    return __builtin_assume_aligned(
        user_pointer,
        alignment
    );
}


// TODO: here
void * malign_realloc(void *p, size_t size) {

    malloc_metadata *real_block = (malloc_metadata *) (((uint8_t *)p) - (
        sizeof(malloc_metadata)
    ));

#if DEBUG
    printf("user-address: %p\n", p);
    printf("calc address: %p\n", (void *)real_block);
#endif

    if (UNLIKELY(size == real_block->user_size))
        return p;

    void *user_memory = malign_alloc(size, (posix_alignments)real_block->user_alignment);
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

void malign_free(void *p) {

    malloc_metadata *real_block = (malloc_metadata *) (((uint8_t *)p) - (
            sizeof(malloc_metadata)
    ));

#if DEBUG
    printf("user-address: %p\n", p);
    printf("calc address: %p\n", (void *)real_block);
#endif

    free(real_block);
}