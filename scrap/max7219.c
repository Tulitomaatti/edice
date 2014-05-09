/* pseudo c serial draft for max7219 */
#include <stdint.h>

int sendByte(uint8_t data, uint8_t address) {
    uint16_t serial_packet = 0; 
    uint8_t i = 0;

    // combine data and address to 16 bit packet 
    serial_packet |= (address << 8);
    serial_packet |= data;    

    setLoadPin();

    for (i = 0; i < 16; i++) {

        waitEnough();
        
        setDataPin( (serial_packet >> (15-i) & 0x01 ) ); 

        clockUp();
        waitEnough();  // 800 hz? 
        clockDown(); 
    }

    resetLoadPin();

    waitEnough();
}
