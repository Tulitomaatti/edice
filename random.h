// random.h

#ifndef RANDOM_H
#define RANDOM_H

#include <stdint.h>

#define TINYMT_RAND_MAX 4294967295UL
#define EIGHT_BIT_RAND_MAX 0xFFFF



uint8_t init_tinymt(uint32_t seed);
uint32_t random_32int();
// uint8_t random_uint8();
// uint8_t random_uint8_range(uint8_t min_inclusive, uint8_t max_inclusive);



#endif
