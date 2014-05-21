// serial.h


#include <stdint.h>

#ifndef SERIAL_H
#define SERIAL_H

#define BAUDS 38400

#ifndef __AVR_ATmega328__
#define __AVR_ATmega328__
#define F_CPU 16000000UL
#endif



#define FOSC F_CPU // Clock Speed
#define BAUD BAUDS
#define MYUBRR FOSC/16/BAUD-1

void USART_Init(uint16_t ubrr_value);
void USART_Transmit(uint8_t data);


void transmit_freeRam();
void transmit_32(uint32_t data);

#endif


