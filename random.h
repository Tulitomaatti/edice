// random.h

#ifndef RANDOM_H
#define RANDOM_H

#include <stdint.h>

// #define RANDOM_MAX 0xFFFF

uint8_t random_int();
uint32_t random_32int();
uint8_t init_tinymt(uint32_t seed);


#endif