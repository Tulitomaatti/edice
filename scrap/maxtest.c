/* maxtest */
/* headers for max7221/7219 */

#define __AVR_ATtiny2313__
#define F_CPU 1000000UL

#include <inttypes.h>
#include <avr/io.h>
#include <util/delay.h>
#include "max7221.h"

volatile uint8_t display[8] = {0};

int main() {
    uint8_t counter;
    uint8_t numbrar[8] = {1, 2, 3, 4, 5, 6, 7, 8}; 


    DDRB = 0xFF; 
    PORTB = 0xFF;


    // leave shutdown mode 
    maxSend16bits(0xFCFF);

    maxSend8bits(0xAA, INTENSITY_ADDR);
    maxSend8bits(0xFF, DIGIT_1);
    maxSend8bits(0xFF, SCAN_LIMIT_ADDR);
    _delay_ms(1);

    counter = 0;
    while (1) {
        //maxSetIntensity(counter);
        maxSend8bits(counter, INTENSITY_ADDR);
        //maxDisplayNumber(counter % 10, 1);
        maxDisplayFigure(numbrar);
        // maxSend8bits(_BV((counter%8)), DIGIT_1);
        counter ++;

        numbrar[counter % 8] = (numbrar[counter % 8] % 10) + 1; 

    }
}