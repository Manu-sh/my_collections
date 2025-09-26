#pragma once
#include <stdint.h>

#include "align_common.h"
#include "malign_meta.h"

#define __helper static FORCED(inline)


__helper malign_metadata ** __get_meta(void *user_pointer) {

    // gli 8 byte prima di user_pointer sono un puntatore ai metadati
    malign_metadata **metadata = (malign_metadata **) (
        ((uint8_t *)user_pointer) - sizeof(malign_metadata **)
    );

    assert(metadata);
    return metadata;
}

// read metadata pointer
__helper malign_metadata * get_meta(void *user_pointer) {
    return *__get_meta(user_pointer);
}

// set metadata pointer
__helper malign_metadata * set_meta(void *user_pointer, malign_metadata *metadata) {
    return *__get_meta(user_pointer) = metadata, metadata;
}

// move the user pointer back to the begging of the block
__helper void * get_real_block(void *user_pointer, const malign_metadata *metadata) {
    return ((uint8_t *)user_pointer) - metadata->offset;
}

__helper uint64_t get_real_block_size(void *user_pointer, const malign_metadata *metadata) {    // real_block: [opt_pad + metadata + user_block]
    const uint8_t *const end = (((uint8_t *)user_pointer) + malign_meta_user_block_aligned_size(metadata)); // move at the end of user_block which is also the end of real_block
    const uint8_t *const beg = ((uint8_t *)get_real_block(user_pointer, metadata));             // get the beginning of the real_block
    return ((uintptr_t)end) - ((uintptr_t)beg);
}

// calcola l'offset dell'user_pointer rispetto a real_block perché l'indirizzo abbia un dato allineamento metadata->alignment
// void *user_pointer = real_block + offset;
__helper uint8_t calc_offset(void *real_block, posix_alignment alignment) {
    // alignment will be a pow of 2: a%b -> a&(b-1)
    return align_size(sizeof(malign_metadata **), alignment) + (alignment - (
            ((uintptr_t)real_block) & (alignment - 1) // return alignment - ( ((uintptr_t)real_block) % alignment );
    ));
}

// get an aligned pointer
__helper void * get_user_block(void *real_block, posix_alignment alignment) {
    return ((uint8_t *)real_block) + calc_offset(real_block, alignment);
}
