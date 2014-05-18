// edice.h

#ifndef EDICE_H
#define EDICE_H

#ifndef __AVR_ATmega328__
#define __AVR_ATmega328__
#define F_CPU 16000000UL // max 16MHz for atmega328p, default 1Mhz 
#endif 

#define BAD_SEED 0
#define RNG_NOT_READY 0
#define RNG_READY 1

#define ENC_COUNTER_TUNE_FACTOR 1

#define RESULTS_START_DIGIT 1
#define DIE_SIZE_START_DIGIT 7
#define N_OF_DICE_START_DIGIT 5

#define MAX_DIE_SIZE 99
#define MIN_DIE_SIZE 2 
#define MAX_THROWS 99 
#define MIN_THROWS 1

#define RANDOMNESS_TESTING_MODE 0


#include <stdint.h> 


void display_update_timer_setup();
void update_results();
// void cool_visual_effects(uint8_t count);

void preinit_RNG();
void finalize_RNG_init();
void reinit_RNG();
void results_scroller_timer_setup();

uint8_t results_changed();
void update_displays();

void init();
void pin_setup();
void adc_setup();
void serial_comm_setup();

void polling_timer_setup();

void display_setup(uint8_t brightness);

void check_inputs();
// void throw_dice();


int freeRam();
void transmit_freeRam();

#endif 
