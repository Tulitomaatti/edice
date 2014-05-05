/* MAX7221 helper functions */

// #define __AVR_ATtiny2313__

#include "max7221.h"
#include <inttypes.h> 
#include <avr/io.h>
#include <util/delay.h>
#include <math.h>
#include <stdlib.h> 

static const uint8_t numbers[12] = {NUMBERS};
static const uint8_t digits[NUMBER_OF_DIGITS] = {DIGITS};
static uint8_t leading_zero_suppression = 1;

/* Functions for sending data */
void maxSend16bits(uint16_t data) {
    uint8_t i;
// clear clock and load bit
    PORTC &= ~(_BV(CLK_PIN) | _BV(LOAD_PIN));

    // bit bang data in. 
    for (i = 0; i < 16; i++) {
        // MSB goes in first. 
        if ( (0x01 & (data >> (15-i))) ) {
            PORTC |= _BV(DATA_PIN); 
        } else {
            PORTC &= ~_BV(DATA_PIN);
        }

        // seems to be stable even without this. 
        _delay_loop_1(DATA_DELAY);

        // Tick Tock
        PORTC |= _BV(CLK_PIN);
        _delay_loop_2(CLK_PULSE * HICLOCK_ADJUST); // makes clock look nicer on scope.
        PORTC &= ~_BV(CLK_PIN);    
    }

    // set load bit to latch data. 
    PORTC |= 0x01;
    
    // clear clock and data (not mandatory, maybe.)
    _delay_loop_1(CLK_PULSE); // might work without this one.
    PORTC &= ~(_BV(DATA_PIN) | _BV(CLK_PIN));
    _delay_loop_1(CLK_PULSE); // should work without this one.

}

void maxSend8bits(uint8_t data, uint8_t address) {
    // uint16_t aux = 0;
    // aux = (address << 8) | data;
    maxSend16bits((address << 8) | data);
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
    PORTC = 0xFF;

    maxExitTestMode();
    maxSend8bits(0xFF, SCAN_LIMIT_ADDR);
    maxSetDecodeMode(0);
    maxSetIntensity(0);
    maxUnShutdown();

}

void maxSetIntensity(uint8_t intensity) {
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

void maxDisplayNumbers(uint8_t numbers[NUMBER_OF_DIGITS]) {
    uint8_t i;
    for (i = 0; i < NUMBER_OF_DIGITS; i++) {
        maxDisplayNumber( numbers[i], digits[i]);
    }
}

int maxDisplayFigure(uint32_t figure, uint8_t *display, uint8_t start_digit, uint8_t len) {
    uint8_t i;
    uint32_t limit;

    // sanity checks and trimming
    if (start_digit + len > NUMBER_OF_DIGITS + 1) {
        return 1;
    }

    start_digit -= 1;

    limit = 1;
    for (i = 0; i < len; i++) {
        limit *= 10;
    }

    figure = figure % limit;

    // Insert the figure to the display var and update display
    for (i = start_digit + len; i > start_digit; i--) {
        
        if (!figure) {  
            if (leading_zero_suppression) {
                display[i - 1] = BLANK_INDEX; 
            } else {
                display[i - 1] = figure;
            }

        } else {
            display[i - 1] = figure % 10;
            figure /= 10;                                                                         
        }
    }


    maxDisplayNumbers(display);
    return 0;
}



// uint8_t numberOfDecimalDigits(uint32_t x) {
//     uint8_t n, i;
//     for (i = 10, n = 1; i < 1000000000; i *= 10, n++) {
//         if (x < i) return n;
//     }
// }
