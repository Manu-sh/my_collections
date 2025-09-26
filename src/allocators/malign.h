#pragma once
#include <stdint.h>

#include "align_common.h"
#include "malign_meta.h"
#include "malign_helpers.h"

// TODO: non fa il check dell'overflow sulle moltiplicazioni

/*
    vincoli rispettati:
        - user_pointer punta ad un blocco che è grande un multiplo di alignment
        - user_pointer è un'indirizzo multiplo di alignment

        - user_pointer - sizeof(void *) è sempre accessibile ed è un puntatore,
           siccome ho bisogno di poterne modificare l'indirizzo devo però trattarlo come doppio puntatore


        alignment = 8 byte

        24 byte block:
            reserved_block = align_size(sizeof(void **), alignment) // 8
            malloc( reserved_block + alignment + align_size(size, alignment) ) // + alignment is because the pointer could be misaligned and we need to adjust his address

        real_blk = (0x07, ..., 0x1e) -> (0x07, 0x1f]

        new_offset   = reserved_block + (alignment - ( ((uintptr_t)real_blk) % alignment )) -> 9
        user_pointer = real_blk + new_offset -> 0x10
        real_blk     = user_pointer - new_offset -> (0x10-9) = 0x07

        0x1e - 0x10 -> 15 byte (0x1e - 0x10 = 14 ma siccome 0x1e non è end ma un'indirizzo accessibile sono 15 byte)

        align_size riporta la size del blocco ma se fosse 14 memcpy non farebbe ottimizzazioni,
        per questo l'helper riporta la size richiesta dall'utente ma allineata.

        alignment = 32 byte

        96 byte block:
            reserved_block = align_size(sizeof(void **), alignment) // 32
            malloc( reserved_block + alignment + align_size(size, alignment) ) // + alignment is because the pointer could be misaligned and we need to adjust his address

        real_blk = (0x07, ..., 0x66) -> (0x07, 0x67]

        new_offset   = reserved_block + (alignment - ( ((uintptr_t)real_blk) % alignment )) -> 57
        user_pointer = real_blk + new_offset -> 0x40
        real_blk     = user_pointer - new_offset -> (0x40-57) = 0x07

        0x67-0x40 -> 39 byte

    c'è il fatto che malloc() su linux ritorna indirizzi multipli di 8 quindi per testare bisognerebbe fare un mock() di malloc
    sfruttando .data oppure allocando un blocco grosso con malloc e restituendo indirizzi disallineati

*/


// [opt-padding + pointer-metadata][user-memory][opt-padding]
void * malign_alloc(uint64_t size, posix_alignment alignment) {

    assert(alignment >= sizeof(malign_metadata **)); // enough space for metadata pointer

    // padding + reserved + aligned block: padding is required to move the user_pointer at the right address to be aligned
    const uint64_t reserved_block = align_size(sizeof(malign_metadata **), alignment);
    const uint64_t aligned_size   = align_size(size, alignment);
    const uint64_t real_size      = reserved_block + alignment + aligned_size;

    uint8_t *real_block = (uint8_t *)malloc(real_size);
    if (UNLIKELY(!real_block))
        return NULL;

    #ifdef DEBUG
        printf("%s real_size=%lu\n", __func__, real_size);
    #endif

    // how many bytes i have to skip to have an address which is multiple of alignment
    // questo valore va salvato, non posso sapere quanti byte ho skippato
    void *user_pointer = get_user_block(real_block, alignment);

    #ifdef DEBUG
        printf("%s real-block-address: %p\n", __func__, real_block);
        printf("%s user-block-address: %p\n", __func__, user_pointer);
        printf("%s offset=%hu\n", __func__, calc_offset(real_block, alignment));
    #endif

    assert( ((uintptr_t)((uint8_t *)user_pointer)) % alignment == 0); // ensure the pointer is properly aligned
    assert( aligned_size % alignment == 0 ); // ensure the aligned size is aligned
    assert( (((uint8_t *)user_pointer) + aligned_size) <= (real_block + real_size) ); // ensure user can access the aligned size

    // gli 8 byte prima di user_pointer sono un puntatore ai metadati
    set_meta(user_pointer, malign_meta_new(size, alignment, calc_offset(real_block, alignment)));

    if (UNLIKELY(!get_meta(user_pointer))) {
        free(real_block);
        return NULL;
    }

    #ifdef DEBUG
        printf("%s real-block-address: %p\n", __func__, real_block);
        printf("%s meta-address: %p\n", __func__, (void *)get_meta(user_pointer));
        printf("%s user-block-address: %p\n", __func__, user_pointer);
        printf("%s user-block-alignment: %hu\n", __func__, get_meta(user_pointer)->requested_alignment);
        printf("%s user-block-size: %lu\n", __func__, get_meta(user_pointer)->requested_size);
        printf("%s user-block-aligned-size: %lu\n", __func__, malign_meta_user_block_aligned_size(get_meta(user_pointer)));
        printf("%s offset: %hu\n", __func__, get_meta(user_pointer)->offset);
        puts("");
    #endif

    return __builtin_assume_aligned(
        user_pointer,
        alignment
    );

}


void * malign_realloc(void *user_pointer, uint64_t size) {

    // gli 8 byte prima di user_pointer sono un puntatore ai metadati
    const malign_metadata *metadata = get_meta(user_pointer);
    if (UNLIKELY(size == metadata->requested_size))
        return user_pointer;

    // se l'utente richiede più memoria ma questa è comunque meno della size allineata allora non fare nulla
    // anche se size < malign_meta_user_block_aligned_size() e size > (metadata->requested_size) non bisogna fare niente eccetto aggiornare requested_size
    // es. ho chiesto 60 byte l'userblock ha size allineata a 64, l'utente richiede di allargarlo a 63 la size allineata è 64 quindi non faccio niente
    if (size > metadata->requested_size && size < malign_meta_user_block_aligned_size(get_meta(user_pointer))) {
        ((malign_metadata *)metadata)->requested_size = size; // edit the requested_size
        return user_pointer;
    }

    // get the begging of the block
    void *real_block = get_real_block(user_pointer, metadata);

    #ifdef DEBUG
        printf("%s real-block-address: %p\n", __func__, real_block);
        printf("%s meta-address: %p\n", __func__, (void *)metadata);
        printf("%s user-block-address: %p\n", __func__, user_pointer);
        printf("%s user-block-alignment: %hu\n", __func__, metadata->requested_alignment);
        printf("%s user-block-size: %lu\n", __func__, metadata->requested_size);
        printf("%s user-block-aligned-size: %lu\n", __func__, malign_meta_user_block_aligned_size(metadata));
        printf("%s user-block-real-size: %lu\n", __func__, get_real_block_size(user_pointer, metadata));
        printf("%s offset: %hu\n", __func__, metadata->offset);
        puts("");
    #endif

    // il vecchio blocco va liberato se cambia ma i metadati no
    // non posso usare realloc() perché può ritornare un indirizzo completamente diverso e a quel punto è inutile che abbia chiamato realloc()

    assert(metadata->requested_alignment >= sizeof(void **)); // enough space for metadata pointer

    // padding + reserved + aligned block: padding is required to move the user_pointer at the right address to be aligned
    const posix_alignment alignment      = (posix_alignment)metadata->requested_alignment;
    const uint64_t reserved_block        = align_size(sizeof(malign_metadata **), alignment);
    const uint64_t new_user_aligned_size = align_size(size, alignment);
    const uint64_t new_real_size         = reserved_block + alignment + new_user_aligned_size; // make enough space to have 8 byte reserved pointer + an aligned user pointer to an aligned user block

    #ifdef DEBUG
        printf("%s ask for blk size %lu\n", __func__, new_real_size);
    #endif

    uint8_t *new_real_block = (uint8_t *)malloc(new_real_size);
    if (UNLIKELY(!new_real_block))
        return NULL;

    void *new_user_pointer = get_user_block(new_real_block, alignment);
    set_meta(new_user_pointer, (malign_metadata *)metadata); // assign metadata ptr to new block

    #ifdef DEBUG
        printf("%s src-block-address: %p\n", __func__, user_pointer);
        printf("%s user-block-alignment: %hu\n", __func__, metadata->requested_alignment);
        printf("%s src-block-size: %lu\n", __func__, metadata->requested_size);
        printf("%s src-block-aligned-size: %lu\n", __func__, malign_meta_user_block_aligned_size(metadata));
        printf("%s new-block-real-size: %lu\n", __func__, new_real_size);
    #endif

    // copy user-data into user_memory
    memcpy(
        __builtin_assume_aligned(new_user_pointer, metadata->requested_alignment),
        __builtin_assume_aligned(user_pointer, metadata->requested_alignment),
        MIN( // the new block could be smaller: get the aligned size of the user block
            align_size(metadata->requested_size, (posix_alignment)metadata->requested_alignment),
            new_user_aligned_size
        )
    );

    free(real_block);

    // update offset and size metadata
    malign_metadata *update_metadata = get_meta(new_user_pointer);
    update_metadata->offset          = calc_offset(new_real_block, alignment);
    update_metadata->requested_size  = size;

    #ifdef DEBUG
        printf("%s new-block-address: %p\n", __func__, new_real_block);
        printf("%s meta-address: %p\n", __func__, (void *)update_metadata);
        printf("%s new-user-block-address: %p\n", __func__, new_user_pointer);
        printf("%s new-user-block-alignment: %hu\n", __func__, update_metadata->requested_alignment);
        printf("%s new-user-block-size: %lu\n", __func__, update_metadata->requested_size);
        printf("%s new-user-block-aligned-size: %lu\n", __func__, malign_meta_user_block_aligned_size(update_metadata));
        printf("%s new-offset: %hu\n", __func__, (update_metadata)->offset);
        puts("");
    #endif

    return new_user_pointer;
}


void malign_free(void *user_pointer) {

    malign_metadata *metadata = get_meta(user_pointer);
    void *real_block = get_real_block(user_pointer, metadata);

    #ifdef DEBUG
        printf("%s block-address: %p\n", __func__, real_block);
        printf("%s meta-address: %p\n", __func__, (void *)metadata);
        printf("%s user-address: %p\n", __func__, user_pointer);
        printf("%s user-alignment: %hu\n", __func__, metadata->requested_alignment);
        printf("%s user-size: %lu\n", __func__, metadata->requested_size);
        printf("%s user-block-aligned-size: %lu\n", __func__, malign_meta_user_block_aligned_size(metadata));
        printf("%s offset: %hu\n", __func__, metadata->offset);
        puts("");
    #endif

    malign_meta_free(metadata); // bye :(
    free(real_block);
}
