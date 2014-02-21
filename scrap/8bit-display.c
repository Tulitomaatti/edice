/* 10 bit display for 10 bit numbers.  */ 

#define F_CPU 16000000UL


#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include <avr/interrupt.h>
#include <stdlib.h>

/* These are pins 2 to 11 on arduino uno. */
/* uint8_t pins[10] = {PORTD2, PORTD3, PORTD4, PORTD5, PORTD6, PORTD7, PORTB0, PORTB1, PORTB2, PORTB3};
   ^ wasn't a good idea, can't access pins straight: must use the whole port. 
*/

uint8_t pinInit() {
    /* Set ports D & B to output, and set them low. */
    DDRB = 0xFF;
    DDRD = 0xFF; 

    PORTB = 0x00;
    PORTD = 0x00; 

    /* set portc to input. */
    ADMUX |= (1 << REFS0); // vref = avcc
    // prescaler to 128 and enable adc... what??
    ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0) | (1 << ADEN);

    /* set pin 13 to input  */
    /* pin 13 is PORTB5, already set. */
    /* PCINT5 is on pin 13. */ 

    SREG |= 1 << SREG_I;

    PCICR |= _BV(PCIE0);
    PCMSK0 |= _BV(PCINT4);

    sei();

    return 0;
}

/* readadc from a tutorial: seems to work.  */ 
uint16_t ReadADC(uint8_t ADCchannel)
{
     //select ADC channel with safety mask
     ADMUX = (ADMUX & 0xF0) | (ADCchannel & 0x0F);
     //single conversion mode
     ADCSRA |= (1<<ADSC);
     // wait until ADC conversion is complete
     while( ADCSRA & (1<<ADSC) );
     return ADC;
}


uint8_t display10bit(uint16_t n) {
    int error = 0;
    int i = 0;

    if (n > 1023) error = 1;

    for (i = 0; i < 10; i++) {
        if (n >> i & 1) /* if the ith bit is 1, set it. */ {
            if (i < 6) {
                PORTD |= 1 << (i + 2); 
            } else {
                PORTB |= 1 << (i - 6);
            } /* 2 and 6 come from port/pin numbering of atmega328 */
        } else { /* clear it. */
            if (i < 6) {
                PORTD &= ~(1 << (i + 2)); 
            } else {
                PORTB &= ~(1 << (i - 6));
            }
        }

    }
    
    return error;
}


ISR(PCINT0_vect) {
    if ((PINB << 3) & 1 == 1) reti();
    _delay_ms(1000); // software debounce.
    display10bit(random());
    _delay_ms(1000);


}


int main () {
    //uint16_t counter = 0;  
    uint8_t channel = 0x01;
    uint16_t seed = 0;
    pinInit();
    seed = ReadADC(channel);
    srand(seed);


    // display10bit(0xAD);

    // char counter;
    // /* set PORTB to output... whatever that means. */
    // /* DDR = data direction register,
    // FF = high = output, 
    // 00 = low = input. 
    // */ 

    // /* PINA contains whatever pin A contains; can be read. */

    // /* PORTx is used to output data to port x. */
    //  PORTx contains 8 bits, individual bits can be accessed with
    // PORTx.# where # is a number from 0 to 7.  

    // DDRB = 0xFF; 



    while (1) {
        display10bit(ReadADC(channel) );
        _delay_ms(50);
    }


    // while (1) { 
    //     display10bit(counter);
    //     counter++;
    //     _delay_ms(100);
    //     }

    // }

    return 0;
}

/* 

plugging arduino in: 

0 [Level 5] [com.apple.message.domain com.apple.commssw.cdc.device] [com.apple.message.driver AppleUSBCDCACMData] [com.apple.message.vendor_id 0x2341] [com.apple.message.product_id 0x43]
AppleUSBCDCACMData: Version number - 4.1.23, Input buffers 8, Output buffers 16
AppleUSBCDC: Version number - 4.1.23
*/