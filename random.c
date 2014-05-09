// random.c 

#include "tinymt32.h"

static tinymt32_t tinymt;

uint8_t random_int() {
    uint32_t random_bits;
    uint8_t return_bits;

    random_bits = tinymt32_generate_uint32(&tinymt);
    return_bits = (random_bits >> 16); 

    return return_bits;
}

uint32_t random_32int() {
    return tinymt32_generate_uint32(&tinymt);
}



uint8_t init_tinymt(uint32_t seed) {
    // magic numbers from tinymt32dc program. 
    tinymt.mat1 = 0x73aa6214;
    tinymt.mat2 = 0xbfccb6af;
    tinymt.tmat = 0x2acfe7fd;

    tinymt32_init(&tinymt, seed);

    return 0;
}


// magic:
// e63abbe7d7fec47363530af48c53ba81,32,1702128737,
// 73aa6214,
// bfccb6af,
// 2acfe7fd,7
// 1,0
