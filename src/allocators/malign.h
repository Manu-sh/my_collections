#pragma once

#include "align_common.h"
#include "malign_meta.h"
#include <stdint.h>

/*
 * TODO:
    la potenza di 2 è il vincolo sull'indirizzo
    non sul blocco
    io lo sto facendo anche sulla size del blocco
 */
// TODO: non fa il check dell'overflow sulle moltiplicazioni
void * malign_alloc(uint64_t size, posix_alignments alignment) {

    assert(alignment >= sizeof(malign_metadata **)); // enough space for metadata pointer
    const uint64_t malign_blk_size = alignment;

    uint64_t real_size = calc_align_size_based(
        malign_blk_size + size, // reserve size for our metadata pointer
        alignment
    );

    printf("%s real_size=%lu\n", __func__, real_size);

    return NULL;
}