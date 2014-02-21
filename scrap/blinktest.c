/* blink test */ 

#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>

int main (void) {
    char counter;
    /* set PORTB to output... whatever that means. */
    /* DDR = data direction register,
    FF = high = output, 
    00 = low = input. 
    */ 

    /* PINA contains whatever pin A contains; can be read. */

    /* PORTx is used to output data to port x. */
    /* PORTx contains 8 bits, individual bits can be accessed with
    PORTx.# where # is a number from 0 to 7. */ 

    DDRB = 0xFF; 

    while (1) {
        PORTB = 0xFF;

        counter = 0; 
        while (counter != 50) {
            _delay_loop_2(30000);
            counter++;
        }

        PORTB = 0x00;

        counter = 0; 
        while (counter != 50) {
            _delay_loop_2(30000);
            counter++;
        }
    }

    return 1;
}

/* 

plugging arduino in: 

0 [Level 5] [com.apple.message.domain com.apple.commssw.cdc.device] [com.apple.message.driver AppleUSBCDCACMData] [com.apple.message.vendor_id 0x2341] [com.apple.message.product_id 0x43]
AppleUSBCDCACMData: Version number - 4.1.23, Input buffers 8, Output buffers 16
AppleUSBCDC: Version number - 4.1.23
*/