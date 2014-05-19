// serial.c 

#include "serial.h"
#include "random.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>



// from data sheet:
void USART_Init( uint16_t ubrr) {
    /*Set baud rate */
    UBRR0H = (unsigned char)(ubrr>>8);
    UBRR0L = (unsigned char)ubrr;

    /*Enable receiver and transmitter */
    UCSR0B = (1<<RXEN0)|(1<<TXEN0);

    /* Set frame format: 8data, 2stop bit */
    UCSR0C = (1<<USBS0)|(3<<UCSZ00);
}

void USART_Transmit( unsigned char data ) {
    /* Wait for empty transmit buffer */
    while (!( UCSR0A & (1 << UDRE0) ));
    // while ( !( UCSRnA & (1<<UDRE0)) );

    /* Put data into buffer, sends the data */
    UDR0 = data;
}

int freeRam() {
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}

void transmit_freeRam() {
    // static uint8_t counter = 0;
    uint16_t freeram;
    freeram = freeRam();
        
    // USART_Transmit(counter++);
    // USART_Transmit(0xFF);
    // USART_Transmit(0);
    // USART_Transmit(0);


    USART_Transmit(freeram >> 8);
    USART_Transmit(freeram & 0x00FF);
}