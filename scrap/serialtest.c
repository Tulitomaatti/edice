/* serial tests */

#define F_CPU 1000000UL
#define __AVR_ATtiny2313__
#define BAUDS 9600

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

//from data sheet
/* void USARTInit(uint16_t baud) {
    // set baud rate
    

    UBRRH = baud >> 8;
    UBRRL = baud;

    // enable reciever/transitte
    UCSRB = (1 << RXEN) | (1 << TXEN);

    //set format for data frame: 8 data bits, 2 stop bits:
    UCSRC = (1 << USBS) | (3 << UCSZ0);
}*/

static uint8_t a = 0;


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

int main() {
    DDRB = 0xFF;
    PORTB = 0xFF;
    sei();

    _delay_ms(500);
    USARTInit(10, 0, 2);
    _delay_ms(1000);
    while (1) {
        _delay_ms(100);
    }
}

ISR(USART_UDRE_vect) {
    PORTB = ~PINB;
    a++;
    UDR = a;
}