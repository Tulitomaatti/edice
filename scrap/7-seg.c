/* 7 segment / enc testing.  */ 

#define F_CPU 16000000UL
#define __AVR_ATmega328P__


#include <avr/cpufunc.h>


#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include <avr/interrupt.h>
#include <stdlib.h>

volatile char display = 0x0F;
volatile int prev_a = 0x00; 
volatile int prev_b = 0x00;
volatile int state_counter = 0;
volatile int prev_state = 0;

// This seems, after all, the best way to decode rotary encoders.
// A lookup table implementation seems to be accepted as best practice.

// Possible transitions and their interpretation: 
/*

no change -> no movement
both change -> no movement / errors 

ab -> ab meaning
00 00 no movement
00 01 
00 10
00 11 no movement / errors

01 00
01 01 no movement 
01 10 no movement / errors
01 11

10 00 
10 01 no movement / errors 
10 10 no movement
10 11

11 00 no movement / errors
11 01
11 10
11 11 no movement 



*/

void pinSetup() {
    DDRD = 0xFF; //set port D to output
    PORTD = 0x00; // set port D 

    DDRC = 0x00;
    PORTC = 0x00; //turn of internal pullup resistors

    // Enable PCIs for port C
    PCICR |= _BV(PCIE1); // enable PCIE1 (PCINT14..8)
    PCMSK1 = 0x01; // 0000 0011 < enable pci on pcint8 and 9  

    sei();
}


int turned_clockwise() {
    // int a = PINC & 0x01; // first bit, pc0 = a
    int b = PINC & 0x02; // second bit, pc1 = b

    //if (prev_b == b) return 2; //no movement. 

    if (prev_a) { // falling edge
        if (b) {
            return 0;
        } else {
            return 1;
        }
    } else { // rising edge
        if (b) {
            return 1;
        } else {
            return 0;
        }
    }

    // possible states:
    // a   b  
    // 0   0      
    // 0   1   
    // 1   1
    // 1   0

    // when a changes to high:
    // prev_a  a   b   clockwise
    // 0       1   1   1
    // 0       1   0   0

    // when a changes to low: 
    // prev_a  a   b   clockwise?
    // 1       0   0   1   
    // 1       0   1   0   
}

void setCurrentEncoderStateToPreviousState() {
    prev_a = PINC & 0x01;
    prev_b = PINC & 0x02;
}

ISR(PCINT1_vect) {
    cli();
    _delay_ms(10);

   if (turned_clockwise()) {
        state_counter++; 
        setCurrentEncoderStateToPreviousState();

        if ((state_counter - prev_state) > 4) {
            PORTD = ++display;
            prev_state = state_counter;
        }

        _delay_ms(20);
   } else {
        state_counter--; 
        setCurrentEncoderStateToPreviousState();

        if ((state_counter - prev_state) < -4) {
            PORTD = --display;
            prev_state = state_counter;
        }

        _delay_ms(20);
   }

    // int clockwise = turned_clockwise();

    // if (clockwise) {

    //     if (clockwise > 1) { // something weird happened, ignore.
    //         sei();
    //         reti();
    //     } 

    //     PORTD = ++display;
    //     setCurrentEncoderStateToPreviousState();

    // } /*else {
    //     PORTD = --display;
    //     setCurrentEncoderStateToPreviousState();
    // } */
    // _delay_ms(10);
    // sei();
    // reti();
}



int main() {
    // int i = 0;
    pinSetup();
    PORTC = 0xF0;
    if (PORTC )
    // char aux = 0x00;

    // while(1) {
    //     aux = 0x00;
    //     // PORTD = 0xFF; 
    //     for (i = 0; i < 8; ++i)
    //     {
    //         if (aux == 0x00) {
    //             aux++;
    //             PORTD = ~aux;
    //             _delay_ms(100);
    //         }
    //         aux = aux << 1;
    //         PORTD = ~aux;
    //         _delay_ms(100);
    //     }
    while (1) {_NOP();}
}
