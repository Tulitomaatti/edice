/* maxtest */
/* headers for max7221/7219 */

// #define __AVR_ATtiny2313__

#ifndef __AVR_ATmega328__
#define __AVR_ATmega328__
#define F_CPU 16000000UL
#endif

#include <inttypes.h>
#include <avr/io.h>
#include <util/delay.h>


#ifndef MAX7221_H
#define MAX7221_H

/*For portc tripwires */
// #define CS_PIN 0
// #define CLK_PIN 1
// #define DATA_PIN 2

#define BITBANG_DDR DDRB

#define BITBANG_PORT PORTB // BITBANG_PORT for pcb lines. portc for tripwire lines. 

// for BITBANG_PORT (on miso/mosi/sck pins)
#define CS_PIN 0
#define CLK_PIN 3
#define DATA_PIN 5



#define LOAD_PIN CS_PIN

// #define DATA_PORT PORTC

#define CLK_PULSE 20
#define DATA_DELAY 10
#define HICLOCK_ADJUST 1

// /* Register addresses. Only the last 4 bits actually matter;
//    They first 4 have been defined as 0000 here.  */ 
#define NOP_ADDR 0x00
#define DECODE_MODE_ADDR 0x09
#define INTENSITY_ADDR 0x0A
#define SCAN_LIMIT_ADDR 0x0B
#define SHUTDOWN_ADDR 0x0C
#define DISPLAY_TEST_ADDR 0x0F
#define DIGIT_1 0x01
#define DIGIT_2 0x02
#define DIGIT_3 0x03
#define DIGIT_4 0x04
#define DIGIT_5 0x05
#define DIGIT_6 0x06
#define DIGIT_7 0x07
#define DIGIT_8 0x08

// Data register segment bit locations
#define SEGDP 7 
#define SEGA 6
#define SEGB 5
#define SEGC 4
#define SEGD 3
#define SEGE 2
#define SEGF 1
#define SEGG 0

// 7 segment display bitmasks for numbers.
#define DISPLAY_1 (_BV(SEGB) | _BV(SEGC))
#define DISPLAY_2 (_BV(SEGA) | _BV(SEGB) | _BV(SEGG) | _BV(SEGE) | _BV(SEGD))
#define DISPLAY_3 (_BV(SEGA) | _BV(SEGB) | _BV(SEGG) | _BV(SEGC) | _BV(SEGD))
#define DISPLAY_4 (_BV(SEGF) | _BV(SEGB) | _BV(SEGG) | _BV(SEGC))
#define DISPLAY_5 (_BV(SEGA) | _BV(SEGF) | _BV(SEGG) | _BV(SEGC) | _BV(SEGD))
#define DISPLAY_6 (_BV(SEGA) | _BV(SEGC) | _BV(SEGD) | _BV(SEGE) | _BV(SEGF) | _BV(SEGG))
#define DISPLAY_7 (_BV(SEGA) | _BV(SEGB) | _BV(SEGC))
#define DISPLAY_8 (_BV(SEGA) | _BV(SEGB) | _BV(SEGC) | _BV(SEGD) | _BV(SEGE) | _BV(SEGF) | _BV(SEGG))
#define DISPLAY_9 (_BV(SEGA) | _BV(SEGB) | _BV(SEGC) | _BV(SEGF) | _BV(SEGG))
#define DISPLAY_0 (_BV(SEGA) | _BV(SEGB) | _BV(SEGC) | _BV(SEGD) | _BV(SEGE) | _BV(SEGF))
#define BLANK 0x00
#define TEST 0xFF

#define NUMBERS DISPLAY_0, DISPLAY_1, DISPLAY_2, DISPLAY_3, DISPLAY_4, DISPLAY_5, DISPLAY_6, DISPLAY_7, DISPLAY_8, DISPLAY_9, BLANK, TEST
#define DIGITS DIGIT_1, DIGIT_2, DIGIT_3, DIGIT_4, DIGIT_5, DIGIT_6, DIGIT_7, DIGIT_8
#define BLANK_INDEX 10
#define TEST_INDEX 11

#define NUMBER_OF_DIGITS 8

// function prototypes: 

void maxSend16bits(uint16_t data);
void maxSend8bits(uint8_t data, uint8_t address);
void maxDisplayNumber(uint8_t number, uint8_t digit, uint8_t enable_dp);
void maxDisplayNumbers(uint8_t numbers[NUMBER_OF_DIGITS]);
int maxDisplayFigure(uint32_t figure, uint8_t start_digit, uint8_t len, uint8_t enable_dp);
void maxShutdown(); 
void maxAddDP(uint8_t digit, volatile uint8_t *display);
void maxUnShutdown(); 
void maxSetDecodeMode(uint8_t mode);
void maxEnterTestMode();
void maxExitTestMode();
void maxSetIntensity(uint8_t intensity);

// // not really fit to be here 
// uint8_t numberOfDecimalDigits(uint32_t x);

#endif

