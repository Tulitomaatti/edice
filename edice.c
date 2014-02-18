/* edice test/main file */

/* ps. i have no idea about coding for hardware. 
   ==> might be a better idea to make a software implementation first. */

#define F_CPU 16000000UL /* 16MHz for atmega328 */
#define BAUD 9600

#define BAD_SEED 0
#define RNG_NOT_READY 0
#define RNG_READY 1

/* #include "/usr/local/CrossPack-AVR/avr/include/avr/interrupt.h" */

#include <avr/interrupt.h>
#include <avr/power.h>
/* #include <avr/sleep.h> */
#include <util/delay.h>
#include <util/setbaud.h>
#include <stdlib.h>
#include <avr/io.h>
#include <stdint.h>
/* #include <time.h> time.h is OS stuff - AVR's don't really have an OS. Use io.h stuff and timers. */ 


/* I can't think of a nice way to pass these to an interrupt function
   other than using file scope variables. */
uint32_t seed = BAD_SEED;
char rng_ready = 0;
clock_t run_time = 0; 


void RNGPreInit(clock_t run_time) {

    /* TODO: Read something from ADC inputs  */     

    seed += (uint32_t) (clock() - run_time);
}

int main() {
    run_time = clock();

    /* do whatever pin initialization needed to be done before touching rng */

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