// port b blinker 

#define __AVR_ATtiny2313__
#define F_CPU 1000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <inttypes.h>

#define DELAY 300

int main() {
    DDRB = 0xFF;

    while (1) {
        PORTB = 0x00;
        _delay_ms(DELAY);
        PORTB = 0xFF;
        _delay_ms(DELAY);
    }

    return 1;
}