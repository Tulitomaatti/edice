// serial.c 

#include "serial.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>



// from some forum: 
void USARTInit(uint16_t ubrr_value, uint8_t x2, uint8_t stopbits) {
    UBRRL = ubrr_value & 0xFF;
    UBRRH = ubrr_value >> 8; 

    // 8 data bits, no parity, asynch.  1 or 2 stop bits. 
    UCSRC = _BV(UCSZ1) | _BV(UCSZ0);
    if (stopbits == 2) UCSRC |= _BV(USBS);

    if (x2) UCSRA = _BV(U2X); //2x

    // usart data registre epty interrupt enable 
    UCSRB = _BV(UDRIE);

    UCSRB |= _BV(RXEN) | _BV(TXEN);
}

void USARTTransmit(uint8_t data) {
    // wait for transmit buffer to free up
    while (! (UCSRA & (1 << UDRE)));
    PORTB = 0x00;

    // put data in the buffer. this sends the data.
    UDR = data;
}