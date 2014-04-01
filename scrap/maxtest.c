/* maxtest */
/* headers for max7221/7219 */



#define __AVR_ATtiny2313__
#define F_CPU 1000000UL

#include <inttypes.h>
#include <avr/io.h>
#include <util/delay.h>
#include "max7221.h"


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
    }
}


// #ifndef MAX7221_H
// #define MAX7221_H

// #define CS_PIN 0
// #define CLK_PIN 1
// #define DATA_PIN 2
// #define LOAD_PIN CS_PIN

// #define CLK_PULSE 20
// #define DATA_DELAY 10
// #define HICLOCK_ADJUST 1

// // /* Register addresses. Only the last 4 bits actually matter;
// //    They have been defined as 0000 here.  */ 
// #define NOP_ADDR 0x00
// #define DECODE_MODE_ADDR 0x09
// #define INTENSITY_ADDR 0x0A
// #define SCAN_LIMIT_ADDR 0x0B
// #define SHUTDOWN_ADDR 0x0C
// #define DISPLAY_TEST_ADDR 0x0F
// #define DIGIT_1 0x01
// #define DIGIT_2 0x02
// #define DIGIT_3 0x03
// #define DIGIT_4 0x04
// #define DIGIT_5 0x05
// #define DIGIT_6 0x06
// #define DIGIT_7 0x07
// #define DIGIT_8 0x08

// // Data register segment bit locations
// #define SEGDP 7 
// #define SEGA 6
// #define SEGB 5
// #define SEGC 4
// #define SEGD 3
// #define SEGE 2
// #define SEGF 1
// #define SEGG 0

// // 7 segment display bitmasks for numbers.
// #define DISPLAY_1 (_BV(SEGB) | _BV(SEGC))
// #define kukka kippo
// #define DISPLAY_2 (_BV(SEGA) | _BV(SEGB) | _BV(SEGG) | _BV(SEGE) | _BV(SEGD))
// #define DISPLAY_3 (_BV(SEGA) | _BV(SEGB) | _BV(SEGG) | _BV(SEGC) | _BV(SEGD))
// #define DISPLAY_4 (_BV(SEGF) | _BV(SEGB) | _BV(SEGG) | _BV(SEGC))
// #define DISPLAY_5 (_BV(SEGA) | _BV(SEGF) | _BV(SEGG) | _BV(SEGC) | _BV(SEGD))
// #define DISPLAY_6 (_BV(SEGA) | _BV(SEGC) | _BV(SEGD) | _BV(SEGE) | _BV(SEGF) | _BV(SEGG))
// #define DISPLAY_7 (_BV(SEGA) | _BV(SEGB) | _BV(SEGC))
// #define DISPLAY_8 (_BV(SEGA) | _BV(SEGB) | _BV(SEGC) | _BV(SEGD) | _BV(SEGE) | _BV(SEGF) | _BV(SEGG))
// #define DISPLAY_9 (_BV(SEGA) | _BV(SEGB) | _BV(SEGC) | _BV(SEGF) | _BV(SEGG))
// #define DISPLAY_0 (_BV(SEGA) | _BV(SEGB) | _BV(SEGC) | _BV(SEGD) | _BV(SEGE) | _BV(SEGF))
// #define BLANK 0x00
// #define TEST 0xFF

// #define NUMBERS DISPLAY_0, DISPLAY_1, DISPLAY_2, DISPLAY_3, DISPLAY_4, DISPLAY_5, DISPLAY_6, DISPLAY_7, DISPLAY_8, DISPLAY_9, BLANK, TEST
// #define DIGITS DIGIT_1, DIGIT_2, DIGIT_3, DIGIT_4, DIGIT_5, DIGIT_6, DIGIT_7, DIGIT_8

// // // function prototypes: 

// void maxSend16bits(uint16_t data);
// void maxSend8bits(uint8_t data, uint8_t address);
// void maxDisplayNumber(uint8_t number, uint8_t digit);
// void maxDisplayFigure(uint8_t *numbers);
// void maxSetup();
// void maxShutdown(); 
// void maxUnShutdown(); 
// void maxSetDecodeMode(uint8_t mode);
// void maxEnterTestMode();
// void maxExitTestMode();
// void maxSetIntensity(uint8_t intensity);

// // // not really fit to be here 
// uint8_t numberOfDecimalDigits(uint32_t x);

// #endif



// static const uint8_t numbers[12] = {NUMBERS};
// static const uint8_t digits[8] = {DIGITS};
// static uint8_t leading_zero_suppression = 0;

// /* Functions for sending data */
// void maxSend16bits(uint16_t data) {
//     uint8_t i;
// // clear clock and load bit
//     PORTB &= ~(_BV(CLK_PIN) | _BV(LOAD_PIN));

//     // bit bang data in. 
//     for (i = 0; i < 16; i++) {
//         // MSB goes in first. 
//         if ( (0x01 & (data >> (15-i))) ) {
//             PORTB |= _BV(DATA_PIN); 
//         } else {
//             PORTB &= ~_BV(DATA_PIN);
//         }

//         // seems to be stable even without this. 
//         _delay_loop_1(DATA_DELAY);

//         // Tick Tock
//         PORTB |= _BV(CLK_PIN);
//         _delay_loop_2(CLK_PULSE * HICLOCK_ADJUST); // makes clock look nicer on scope.
//         PORTB &= ~_BV(CLK_PIN);    
//     }

//     // set load bit to latch data. 
//     PORTB |= 0x01;
    
//     // clear clock and data (not mandatory, maybe.)
//     _delay_loop_1(CLK_PULSE); // might work without this one.
//     PORTB &= ~(_BV(DATA_PIN) | _BV(CLK_PIN));
//     _delay_loop_1(CLK_PULSE); // should work without this one.

// }

// void maxSend8bits(uint8_t data, uint8_t address) {
//     // uint16_t aux = 0;
//     // aux = (address << 8) | data;
//     maxSend16bits((address << 8) | data);
// }


// void maxSetup() {
//     // something ? 
//     /* From data sheet:
//         On initial power-up, all control registers are reset, the
//         display is blanked, and the MAX7219/MAX7221 enter
//         shutdown mode. Program the display driver prior to
//         display use. Otherwise, it will initially be set to scan one
//         digit, it will not decode data in the data registers, and
//         the intensity register will be set to its minimum value.*/
   
//     // Just set everything high for now. 
//     PORTB = 0xFF;

//     maxExitTestMode();
//     maxSend8bits(0xFF, SCAN_LIMIT_ADDR);
//     maxSetDecodeMode(0);
//     maxSetIntensity(0);
//     maxUnShutdown();

// }

// void maxSetIntensity(uint8_t intensity) {
//     // intensity is controller by last 4 bits, 0000 being minimum. 
//     maxSend8bits(intensity, INTENSITY_ADDR);
// }


// void maxShutdown() {
//     maxSend8bits(0, SHUTDOWN_ADDR);
// }

// void maxUnShutdown() {
//     maxSend8bits(1, SHUTDOWN_ADDR);
// }

// void maxSetDecodeMode(uint8_t mode) {
//     // 1 for Code B decode. 
//     // selectable for each digit, for now just on/off is implemented.
//     if (mode) maxSend8bits(0xFF, DECODE_MODE_ADDR);
//     else maxSend8bits(0x00, DECODE_MODE_ADDR);
// }

// void maxEnterTestMode() {
//     maxSend8bits(1, DISPLAY_TEST_ADDR);
// }
// void maxExitTestMode() {
//     maxSend8bits(0, DISPLAY_TEST_ADDR);

// };

// void maxDisplayNumber(uint8_t number, uint8_t digit) {
//     maxSend8bits(numbers[number], digits[digit-1]); 
// }

// void maxDisplayFigure(uint8_t numbers[8]) {
//     uint8_t i;
//     for (i = 0; i < 8; i++) {
//         maxDisplayNumber( numbers[i], digits[i]);
//     }


//     // crashes on attiny2313? stack space? 
//     // prune figure to 0 ~ 9999 9999
//     // uint8_t digits = numberOfDecimalDigits(figure);
//     // figure = figure % 100000000;

//     // if (leading_zero_suppression) {
//     //     for (i = 8; i > digits; i--) {
//     //         maxDisplayNumber(BLANK, i);
//     //     }
//     //     for (i = 0; i < digits; i++) {
//     //         maxDisplayNumber(figure % 10, i);
//     //         figure /= 10; 
//     //     }
//     // } else {
//     //     for (i = 0; i < 8; i++) {
//     //         maxDisplayNumber(figure % 10, i);
//     //         figure /= 10; 
//     //     }
//     // }


// }

// uint8_t numberOfDecimalDigits(uint32_t x) {
//     uint8_t n, i;
//     for (i = 10, n = 1; i < 1000000000; i *= 10, n++) {
//         if (x < i) return n;
//     }
// }



