// random.c 

#include "tinymt32.h"
#include "random.h"
#include "serial.h"


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


// Is the chip borked?

// uint8_t random_uint8() {
//     static uint32_t random_bits = 3;

//     random_bits++;
//     random_bits *= 2;
//     random_bits++; 
//     random_bits = random_bits ^ 0xAAAAAAAA;
//     random_bits >>= 3;

//     // static uint8_t counter = 0;
//     // uint8_t random_8_bits;
//     // if (!counter) random_bits = tinymt32_generate_uint32(&tinymt);

//     // random_8_bits = random_bits & 0x000000FF;
//     //     random_bits >>= 8;

//     // if (++counter > 4) counter = 0;

//     // return random_8_bits;
//     return 4;

// }

uint8_t random_uint8_range(uint8_t min_inclusive, uint8_t max_inclusive) {
    uint8_t range = max_inclusive - min_inclusive;
    uint8_t interval = EIGHT_BIT_RAND_MAX / (range + 1);
    uint8_t random_number;

    do {
        transmit_freeRam();
        random_number = (random_32int() & 0xFF) / interval;
    } while (random_number > range);

    return random_number + min_inclusive;

}


// magic parameters from parameter generator:
// e63abbe7d7fec47363530af48c53ba81,32,1702128737,
// 73aa6214,
// bfccb6af,
// 2acfe7fd,7
// 1,0
