// encoders.h

#ifndef ENCODERS_H
#define ENCODERS_H

#ifndef __AVR_ATmega328__
#define __AVR_ATmega328__
#define F_CPU 16000000UL
#endif

#include <avr/io.h>


// E:07, H:D9, L:E0)
// external clock fuses. 

// default fuses:
// (E:07, H:D9, L:62)


// most aren't used I think
// #define HIGH_BITS 0xF0
// #define LOW_BITS 0x0F

// #define ENCODER_PIN PORTD
// #define ENCODER_1A 0
// #define ENCODER_1B 1
// #define ENCODER_2A 2
// #define ENCODER_2B 3


// uint8_t readEncoderState();
// uint8_t decodeEncodersState(uint8_t state, uint8_t encoder_select);

uint8_t new_decode_encoder(uint8_t state);

#endif
