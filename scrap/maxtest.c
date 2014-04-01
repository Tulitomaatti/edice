/* maxtest */

#define F_CPU 1000000UL
#define __AVR_ATtiny2313_

#include <avr/io.h>
#include <stdlib.h>
#include <util/delay.h>
#include "max7221.h"

int main() {
    uint32_t counter = 0;
    maxSetup();

    _delay_ms(1000);
    maxEnterTestMode();
    _delay_ms(1000);
    maxExitTestMode();

    while (1) {
        maxDisplayFigure(counter);
        counter++; 
        _delay_ms(500);
    }


}