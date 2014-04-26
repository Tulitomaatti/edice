// serial.h


#include <stdint.h>

#define BAUDS 9600

#ifndef __AVR_ATmega328__
#define __AVR_ATmega328__
#define F_CPU 1000000UL
#endif


void USARTInit(uint16_t ubrr_value, uint8_t x2, uint8_t stopbits);
void USARTTransmit(uint8_t data);