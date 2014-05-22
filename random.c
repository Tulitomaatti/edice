// random.c 

#include "tinymt32.h"
#include "random.h"
#include "serial.h"
#include <avr/cpufunc.h> 
#include <avr/io.h>
#include "edice.h"

static tinymt32_t tinymt;

uint8_t random_int() {
    uint32_t random_bits;
    uint8_t return_bits;

    random_bits = tinymt32_generate_uint32(&tinymt);
    return_bits = (random_bits >> 16); 

    return return_bits;
}

uint32_t random_32int() {
// 	//extern uint16_t timer1_overflow_counter;
// 	uint32_t randomness = 0;
// // 	rand = TCNT1;
// // 	rand <<= 16;
// // 	rand ^= timer1_overflow_counter;
//     randomness ^= tinymt32_generate_uint32(&tinymt);
// 	randomness ^= get_analog_noise(1);
// 	
// 	return randomness;

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

uint8_t random_uint8() {
	static uint8_t counter = 1;
	uint8_t random_8_bits;
    static uint32_t random_bits;

    if (counter == 1) random_bits = tinymt32_generate_uint32(&tinymt);

    random_8_bits = random_bits & 0x000000FF;
    random_bits >>= 8;
	
	counter++;
    if (counter == 4) counter = 1;
	
// 	random_8_bits ^= TCNT1;
// 	random_8_bits ^= get_analog_noise_8(1);

	return random_8_bits;
}

uint8_t scale_random_uint8_range(uint8_t max_inclusive) { // returns random number [1, max_inclusive]
	max_inclusive--;

    uint8_t interval = EIGHT_BIT_RAND_MAX / (max_inclusive + 1);
    uint8_t random_number;

    do {
        random_number = random_uint8() / interval;
		//random_number = (random_32int() & 0x000000FF) / interval;
    } while (random_number > max_inclusive);

    return random_number + 1;
}


// magic parameters from parameter generator:
// e63abbe7d7fec47363530af48c53ba81,32,1702128737,
// 73aa6214,
// bfccb6af,
// 2acfe7fd,7
// 1,0
