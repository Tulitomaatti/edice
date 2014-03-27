/* 10 bit display for 10 bit numbers.  */ 

#define F_CPU 16000000UL
#define __AVR_ATmega328P__


#include <avr/cpufunc.h>


#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include <avr/interrupt.h>
#include <stdlib.h>

/* These are pins 2 to 11 on arduino uno. */
/* uint8_t pins[10] = {PORTD2, PORTD3, PORTD4, PORTD5, PORTD6, PORTD7, PORTB0, PORTB1, PORTB2, PORTB3};
   ^ wasn't a good idea, can't access pins straight: must use the whole port. 
*/

volatile uint16_t current_number = 0;

uint8_t pinInit() {
    /* Set ports D & B to output, and set them low. */
    DDRB = 0xFF;
    DDRD = 0xFF; 


    PORTB = 0x00;
    PORTD = 0x00; 

    // Use Vcc as the reference voltage in ad conversion. 
    ADMUX |= (1 << REFS0); // vref = avcc

    // prescaler to 128 and enable adc... what??
    ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0) | (1 << ADEN);

    /* Ok so ADCSRA is a register that controls the ad conversion stuff,
       ADPS# are used to set prescaling value (whatever that is) for the adc,
       and the bit at ADEN puts the ADC on.  */

    /* set pin 13 to input  */
    /* pin 13 is PORTB5, already set. */
    /* PCINT5 is on pin 13. */

    // SREG |= 1 << SREG_I; // This supposedly does what sei() does. 

    PCICR |= _BV(PCIE0); /* set bit 0 (PCIE0) to 1, which enables PCI on PCINT0 to PCINT7. */
    PCMSK0 |= _BV(PCINT4);
    PCMSK0 |= _BV(PCINT5); /* PCMSK0 controls PCINT0 to PCINT7 enablation.  */

    /* Though, any PCI from PCINT0 to PCINT7 will trigger PCI0 -interrupt vector. 
        -> which means we have to check which pin it was if it needs to be known. */

    sei(); /* enable global interrupts. */

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

void displayflip() {
    uint8_t i;

    for (i = 0; i < 10; i++) {
        if (i < 6) { // portD + 2 
            // PORTD ^= ((PIND >> (i + 2)) & 1) << (i + 2);
            PORTD ^= 1 << (i + 2);
        } else {
            // PORTB ^= ((PINB >> (i - 6)) & 1) << (i - 6);
            PORTB ^= 1 << (i - 6);

           // PORTB |= 1 << (i - 6);  // portB - 6
        } /* 2 and 6 come from port/pin numbering of atmega328 */
      
    }


}


ISR(PCINT0_vect, ISR_NAKED) {
    cli();
    _delay_ms(100);
    // for (pin = 0; pin < 8; pin++) {
    //     pressed = (PINB >> pin) & 1;
    //     switch (pin) {
    //         case PCINT0:
    //             pin++;
    //         case PCINT1:
    //             pin++;
    //         case PCINT2:
    //             pin++;
    //         case PCINT3:
    //             pin++;
    //         case PCINT4:
    //             if (pressed) break;
    //             _delay_ms(100); // software debounce
    //              current_number = rand();
    //             display10bit(current_number);
    //             break;
    //         case PCINT5:
    //             if (pressed) break;
    //             displayflip();
    //             break;
    //         case PCINT6:
    //             pin++;
    //         case PCINT7:
    //             pin++;
    //     }
    // }

    if (!((PINB >> 4) & 1))  {
        if (((PINB >> 5) & 1)) {
            displayflip();
            reti();
        }
        
    } else {  
        _delay_ms(100); // software debounce
        current_number = rand();
        display10bit(current_number);
        reti();
    }
    reti();
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
    //  PORTx contains 8 bits, individual bits can be accessed with bit shifts & masking.
    // Some macros also exist. 

    // DDRB = 0xFF; 



    while (1) {
        _NOP();
    }


    // while (1) { 
    //     display10bit(counter);
    //     counter++;
    //     _delay_ms(100);
    //     }

    // }

    return 0;
}
