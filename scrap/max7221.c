/* MAX7221 helper functions */

#define __AVR_ATtiny2313__
#define F_CPU 1000000UL

#include <inttypes.h> 
#include <avr/io.h>
#include <util/delay.h>
#include "max7221.h"

static const uint8_t numbers[12] = {NUMBERS};
static const uint8_t digits[8] = {DIGITS};
static uint8_t i;

/* Functions for sending data */
void maxSend16bits(uint8_t data) {
    uint8_t i, bit;
    // MAX7221: put ~CS (chip select) pin low to load data
    PORTB &= ~_BV(CS_PIN); 
    PORTB &= ~_BV(CLK_PIN);

    // Latch data on rising edge of load/cs
    PORTB |= _BV(CS_PIN);

    // clock in data by bitbanging clk and data

    for (i = 0; i < 16; i++) {

        // set data pin and wait.
        bit = 0x01 & (data >> i);
        if (bit) PORTB |= _BV(DATA_PIN);
        else PORTB &= ~_BV(DATA_PIN);

        // clock data in 
        _delay_us(CLK_PULSE);
        PORTB |= _BV(CLK_PIN);

        // set down clock after some time
        _delay_us(CLK_PULSE);
        PORTB &= ~_BV(CLK_PIN);
    }

    // 50 ns delay required before next one
    _delay_us(LOAD_DELAY);
}

void maxSend8bits(uint8_t data, uint8_t address) {
    uint16_t aux = 0;
    aux = (address << 8) | data;
    maxSend16bits(aux);
}

void maxSetup() {
    // something ? 
    /* From data sheet:
        On initial power-up, all control registers are reset, the
        display is blanked, and the MAX7219/MAX7221 enter
        shutdown mode. Program the display driver prior to
        display use. Otherwise, it will initially be set to scan one
        digit, it will not decode data in the data registers, and
        the intensity register will be set to its minimum value.*/
   
    // Just set everything high for now. 
    PORTB = 0xFF;

    maxExitTestMode();
    maxSend8bits(0xff, SCAN_LIMIT_ADDR);
    maxSetDecodeMode(0);
    maxIntensity(0);
    maxUnShutdown();

    // init some numbers in for testing: 
    for (i = 0; i < 8; i++) {
        maxDisplayNumber(i, i);
    }
    
    maxDisplayNumber(3, BLANK);
    maxDisplayNumber(4, TEST);


}

void maxIntensity(uint8_t intensity) {
    // intensity is controller by last 4 bits, 0000 being minimum. 
    maxSend8bits(intensity, INTENSITY_ADDR);
}


void maxShutdown() {
    maxSend8bits(0, SHUTDOWN_ADDR);
}

void maxUnShutdown() {
    maxSend8bits(1, SHUTDOWN_ADDR);
}

void maxSetDecodeMode(uint8_t mode) {
    // 1 for Code B decode. 
    // selectable for each digit, for now just on/off is implemented.
    if (mode) maxSend8bits(0xFF, DECODE_MODE_ADDR);
    else maxSend8bits(0x00, DECODE_MODE_ADDR);
}

void maxEnterTestMode() {
    maxSend8bits(1, DISPLAY_TEST_ADDR);
}
void maxExitTestMode() {
    maxSend8bits(0, DISPLAY_TEST_ADDR);

};

void maxDisplayNumber(uint8_t number, uint8_t digit) {
    maxSend8bits(numbers[number], digits[digit-1]); 
}
