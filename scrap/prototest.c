// program test 

/* 7 segment / enc testing.  */ 

#define F_CPU 1000000UL
#define __AVR_ATtiny2313__
#define BAUDS 9600

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdint.h>
#include <stdlib.h>

#include <avr/sleep.h>


#define HIGH_BITS 0xF0
#define LOW_BITS 0x0F



volatile static uint8_t display;
volatile static uint8_t encoder_state; 
static uint8_t rng_inited;

/* seems to work better than fancy logic math. */
static const uint8_t transition_table[16] = {0,  1, -1,  0,  
                                            -1,  0,  0,  1,
                                             1,  0,  0, -1,
                                             0, -1,  1,  0};

// cw = 1, ccw = -1, error/ no move = 0.

/*
ab -> ab meaning
00    00 no movement
00    01 cw
00    10 ccw 
00    11 no movement / errors

01 00 ccw
01 01 no movement 
01 10 no movement / errors
01 11 cw 

10 00 cw
10 01 no movement / errors 
10 10 no movement
10 11 ccw

11 00 no movement / errors
11 01 ccw 
11 10 cw
11 11 no movement 
*/
    

void pinSetup() {
    // Timer1 (16bit) to 1024 prescale & enable. (XXXX X000 disables.)
    TCCR1B |= 0x01;
    rng_inited = 0;


    /*  DDR   == data direction
        PORT  == write to port, or enable pull up resistors
        PIN   == read a pin
    */

    /* Port B used for LED control */
    DDRB = 0xFF;  // set port b to output (1)
    PORTB = 0x00; 

    /* Use port d for input, and enable internal pull-up
       resistors for pins D0 ~ D3 (0000 1111 = 0x0F 

    actually, use pd0 and pd1 as output. 
       */
    DDRD = 0x03;  // 0000 0011 
    PORTD |= 0xFC; // 1s enable internal pull-up resistor(s)
}

// from attiny data shset
void USARTInit(uint16_t baud) {
    // set baud rate
    UBRRH = baud >> 8;
    UBRRL = baud;

    // enable reciever/transitte
    UCSRB = (1 << RXEN) | (1 << TXEN);

    //set format for data frame: 8 data bits, 2 stop bits:
    UCSRC = (1 << USBS) | (3 << UCSZ0);
}

void USARTTransmit(uint8_t data) {
    // wait for transmit buffer to free up
    while (! (UCSRA & (1 << UDRE)));

    // put data in the buffer. this sends the data.
    UDR = data;
}

uint8_t readEncoderState() {

    // Notes for figuring out a 1-byte data structure for 8 boolean variables. 
    // PIND: 
    // d0 d1 d2 d3 d4 d5 -  -  
    // A1 B1 -- -- A2 B2 -- -- 

    // pind: 
    // -  -  d5 d4 d3 d2 d1 d0
    // -- -- B2 A2 -- -- B1 A1

    //  << 2 : 
    //  b2 a2 -- -- b1 a2 -- -- 
//  OR  (
    //   0  0  1  1  0  0  1  1 AND 
    //  -- -- B2 A2 -- -- B1 A1
    // 
   

    // encoder_state = ((encoder_state << 2) & 0xCC) | (PIND & 0x33); 
    encoder_state = ((encoder_state << 2) & 0xCC) | (PIND & 0x3C); 
   
    // _delay_ms(5); // software debounce    
    // encoder_state |= aux;
    return encoder_state;
}


uint8_t decodeEncodersState(uint8_t state, uint8_t encoder_select) {
    /* let's look only at the last 4 bits. */
    uint8_t index = 0;
    if (encoder_select) {
        index = (state >> 4) & LOW_BITS;
    } else {
        index = state & LOW_BITS;
    }


    return transition_table[index];

}

void ledDisplay(uint8_t x) {
    PORTB = x;
}

void displayflip() {
    display = ~display;
    ledDisplay(display);
}

uint8_t checkFlipButton() {
    return (0x01 & (PIND >> 6));

}

int main() {
    static uint8_t enc0_counter = 0;
    static uint8_t enc1_counter = 0;
    uint8_t random_aux;
    // int i = 0;
    display = 0;
    pinSetup();

    USARTInit(BAUDS);


    while (1) {
        _delay_us(100);

        enc0_counter += decodeEncodersState(readEncoderState(), 0);
        enc1_counter += decodeEncodersState(readEncoderState(), 1);

        // bit shift by one to make them a bit less sensitive. 
        display = ((enc0_counter >> 1) << 4) | ((enc1_counter >> 1) & LOW_BITS); 

        
        if (!checkFlipButton()) {
            if (!rng_inited) {

                displayflip(); 
                _delay_ms(1000);
                srandom(TCNT1);
                rng_inited = 1;
                TCCR1B = 0x00;  // stop timer to save power?

            } else {
                random_aux = (uint8_t) random();
                ledDisplay( random() );
                USARTTransmit(random_aux);

                _delay_ms(100);
            }
        } else {
            ledDisplay( display );
        }
    }
}




