// encoder readout functions 

#include "encoders.h"
#include <stdint.h>
#include <avr/io.h>


// volatile static uint8_t encoder_state; 
// volatile static struct {
//     uint8_t enc1_a : 2;
//     uint8_t enc1_b : 2;
//     uint8_t enc2_a : 2;
//     uint8_t enc2_b : 2;

// } encoder_state_new;

static const uint8_t transition_table[16] = {0,  1, -1,  0,  
                                            -1,  0,  0,  1,
                                             1,  0,  0, -1,
                                             0, -1,  1,  0};

uint8_t new_decode_encoder(uint8_t state) {
    // state = 0000 preva prebv a b
    return transition_table[state & 0x0f];
}



// // deprecated
// uint8_t readEncoderState() {

//     // Notes for figuring out a 1-byte data structure for 8 boolean variables. 
//     // PIND: 
//     // d0 d1 d2 d3 d4 d5 -  -  
//     // A1 B1 -- -- A2 B2 -- -- 

//     // pind: 
//     // -  -  d5 d4 d3 d2 d1 d0
//     // -- -- B2 A2 -- -- B1 A1

//     //  << 2 : 
//     //  b2 a2 -- -- b1 a2 -- -- 
// //  OR  (
//     //   0  0  1  1  0  0  1  1 AND 
//     //  -- -- B2 A2 -- -- B1 A1
//     // 
   

//     // encoder_state = ((encoder_state << 2) & 0xCC) | (PIND & 0x33); 
//     encoder_state = ((encoder_state << 2) & 0xCC) | (ENCODER_PIN & 0x3C); 
   
//     // _delay_ms(5); // software debounce    
//     // encoder_state |= aux;
//     return encoder_state;
// }

// uint8_t decodeEncodersState(uint8_t state, uint8_t encoder_select) {
//     /* let's look only at the last 4 bits. */
//     uint8_t index = 0;
//     if (encoder_select) {
//         index = (state >> 4) & LOW_BITS;
//     } else {
//         index = state & LOW_BITS;
//     }


//     return transition_table[index];
// }