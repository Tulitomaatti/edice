// encoders.h

#ifndef ENCODERS_H
#define ENCODERS_H

#ifndef __AVR_ATmega328__
#define __AVR_ATmega328__
#define F_CPU 1000000UL
#endif

#include <avr/io.h>

#define HIGH_BITS 0xF0
#define LOW_BITS 0x0F

#define ENCODER_PIN PORTD
#define ENCODER_1A 0
#define ENCODER_1B 1
#define ENCODER_2A 2
#define ENCODER_2B 3


#endif
