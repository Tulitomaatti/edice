
#define __AVR_ATtiny2313__
#define F_CPU 1000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <inttypes.h>

#define CLK_PULSE 1
#define DATA_DELAY 1
#define HICLOCK_ADJUST 10

#define CLK_PIN 1
#define LOAD_PIN 0
#define DATA_PIN 2

static uint8_t i;

void sendData(uint16_t data) {

// clear clock and load bit
    PORTB &= ~(_BV(CLK_PIN) | _BV(LOAD_PIN));

    // bit bang data in. 
    for (i = 0; i < 16; i++) {
        // MSB goes in first. 
        if ( (0x01 & (data >> (15-i))) ) {
            PORTB |= _BV(DATA_PIN); 
        } else {
            PORTB &= ~_BV(DATA_PIN);
        }

        // seems to be stable even without this. 
        // _delay_loop_1(DATA_DELAY);

        // Tick Tock
        PORTB |= _BV(CLK_PIN);
        _delay_loop_2(CLK_PULSE * HICLOCK_ADJUST); // makes clock look nicer on scope.
        PORTB &= ~_BV(CLK_PIN);    
    }

    // set load bit to latch data. 
    PORTB |= 0x01;
    
    // clear clock and data (not mandatory, maybe.)
    _delay_loop_1(CLK_PULSE); // might work without this one.
    PORTB &= ~(_BV(DATA_PIN) | _BV(CLK_PIN));
    _delay_loop_1(CLK_PULSE); // should work without this one.


}

int main() {
    uint8_t i;
    uint16_t data, addrmask, datamask; // 1100 1010 1100 0011 in hex
    // reversed: 1100 0011 0101 0011
    DDRB = 0xFF;
    PORTB = 0xFF;

    data = 0;
    addrmask = 0x0F00;
    datamask = 0x00FF;

    // leave shutdown mode 
    _delay_ms(1);

    sendData(0xFCFF);

    _delay_ms(1);
    i = 0;
    while (1) {
        sendData(data = 0x0100 |(datamask & i++) );
   

    }

}