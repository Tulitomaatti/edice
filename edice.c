/* edice test/main file 
 Might be OS X specific, but should work with any avr-libc / avr-gcc combination. 
 ...which means that Atmel's own AVR studio might want different code. 

*/

/* 
TODO: 

Button/switch state/debouncing helpers

Timer interrupts for 
    * Polling button states (pure interrupts make debouncing hard I hear?)
    * Going to power saving/sleep/off modes. 

Display Control
    * Display Logic
    * Serial helpers (7-segment driver or SIPO registers?)

*/ 

/* ps. i have no idea about coding for hardware. 
   ==> might be a better idea to make a software implementation first. */

#define __AVR_ATmega328P__
#define F_CPU 16000000UL /* 16MHz for atmega328p */

#define BAD_SEED 0
#define RNG_NOT_READY 0
#define RNG_READY 1

/* #include "/usr/local/CrossPack-AVR/avr/include/avr/interrupt.h" */

/* #include <avr/sleep.h> */

#include <avr/io.h>
#include <avr/cpufunc.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include <stdlib.h>
#include <stdint.h>

/* #include <time.h> time.h is OS stuff - AVR's don't really have an OS. Use io.h stuff and timers. */ 


/* I can't think of a nice way to pass these to an interrupt function
   other than using file scope variables. */
uint32_t seed = BAD_SEED;
char rng_ready = 0;
uint32_t run_time = 0; 


void RNGPreInit(uint32_t run_time) {

    /* TODO: Read something from ADC inputs  */

    seed += (uint32_t) (clock() - run_time);
}

int main() {
    run_time = clock();



    /* do whatever pin/ADC initialization needed to be done before touching rng */

    RNGPreInit(run_time);
	return 0;
}



void RNGFinalizeInit() {
    seed += (uint32_t) clock();


    /* PRNG seed can be set with srand(uint seed) */
    srand(seed);
    rng_ready = 1;
}

uint8_t throwDice(uint8_t ndice, uint8_t size, uint8_t **results, char rng_ready) {
    uint8_t sum = 0;
    int i = 0;
    
    if (!rng_ready) RNGFinalizeInit();
    

    for (i = 0; i < ndice; i++) {
        (*results)[i] = random(); /* TODO: add appropriate math */
    }

    return sum;
    }