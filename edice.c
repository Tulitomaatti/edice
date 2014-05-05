// edice test/main file 

#include "edice.h"


#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include <stdlib.h>

#include "max7221.h"
#include "encoders.h"
//  #include "serial.h"
// #include random.h // Custom RNG; use stdlib for testing. 


/* Global variables */
volatile uint8_t enc1_count = 2;
volatile uint8_t enc2_count = 12; 
uint8_t encoder1_state = 0; 
uint8_t encoder2_state = 0;

uint8_t die_size = 6;
uint8_t number_of_dice = 1;

uint8_t results[100] = {0};
uint16_t results_sum = 0;

uint8_t display[8] = {0};

volatile struct {
    uint8_t activity : 1;
    uint8_t activity_timeout : 1;
    uint8_t rng_ok : 1;
    uint8_t display_mode : 1;
    uint8_t roll_button_pressed : 1;
    uint8_t enc1_rolled : 1;
    uint8_t enc2_rolled : 1;
    uint8_t something : 1;
} status; 

int main(void) {
    uint8_t i; 
    
    init();

    if (!status.rng_ok) {
        finalize_RNG_init();
    }

    // looping logic: 
    while (1) {
        cli();

        // Update status from inputs
        number_of_dice = ((enc1_count >> ENC_COUNTER_TUNE_FACTOR) % MAX_THROWS) + 1; //>> ENC_COUNTER_TUNE_FACTOR;
        die_size = ((enc2_count >> ENC_COUNTER_TUNE_FACTOR) % MAX_DIE_SIZE) + 1; //>> ENC_COUNTER_TUNE_FACTOR;
        if (die_size == 1) die_size = 2;
        
        // Throw dice, store results, calculate sum. 
        if (status.roll_button_pressed) {            
            results_sum = 0;
            for (i = 0; i < number_of_dice; i++) {
                results_sum += results[i] = (rand() % die_size) + 1; // TODO: Fix: % die_size will slant the distribution. 
            }
        }
       // _delay_ms(10);
        
        

        sei();

        _delay_ms(10);
        
        //_delay_ms(1000);
    }
    

    // TODO: Go to power saving mode, wait for an timer interrupt.


    return 0;
}



void check_inputs() {
    TCCR2B &= ~(_BV(CS20) | _BV(CS21) | _BV(CS22));
    uint8_t input_port = PIND;

    static uint8_t encoder1_state;

    // button
    status.roll_button_pressed = ~(input_port >> 7);

    // encoder 1
    encoder1_state <<= 2; 
    encoder1_state |= (((input_port >> 1) & 0x02) | ((input_port >> 3) & 0x01) ) & 0x0f;
        
    enc1_count += new_decode_encoder(encoder1_state);
    if (enc1_count > 198) enc1_count = 1;
    else if (enc1_count < 1) enc1_count = 198;


    // encoder 2
    encoder2_state <<= 2; 
    encoder2_state |= (((input_port >> 3) & 0x02) | ((input_port >> 5) & 0x01) ) & 0x0f;
        
    enc2_count += new_decode_encoder(encoder2_state);
    if (enc2_count > 198) enc2_count = 1;
    else if (enc2_count < 1) enc2_count = 198;

    // display mode
    status.display_mode = (input_port >> 6) & 1;
    polling_timer_setup();
}


//update displays on every overflow:
ISR(TIMER0_OVF_vect) {
    update_displays();
}

ISR(TIMER0_COMPA_vect) {
    update_displays();
}

ISR(TIMER2_OVF_vect) { 
    check_inputs();
}

void update_displays() {
    // remember last display state to avoid futile updating. 
    static uint8_t last_n_dice;
    static uint8_t last_die_size; 
    static uint8_t last_results_0; 
    static uint8_t last_display_mode;

    if (number_of_dice != last_n_dice) {
        maxDisplayFigure(number_of_dice, display, N_OF_DICE_START_DIGIT, 2); 
        last_n_dice = number_of_dice;
    }
    if (die_size != last_die_size) {
        maxDisplayFigure(die_size, display, DIE_SIZE_START_DIGIT, 2);
        last_die_size = die_size;
    }

    if (last_results_0 != results[0]) {
        if (status.display_mode) {
            // TODO: scroller
            maxDisplayFigure(results[0], display, RESULTS_START_DIGIT, 4);
        } else {
            maxDisplayFigure(results_sum, display, RESULTS_START_DIGIT, 4);
        }
        last_results_0 = results[0];

    } else if (status.display_mode != last_display_mode) {
        if (status.display_mode) {
            // TODO: scroller
            maxDisplayFigure(results[0], display, RESULTS_START_DIGIT, 4);
        } else {
            maxDisplayFigure(results_sum, display, RESULTS_START_DIGIT, 4);
        }
        last_display_mode = status.display_mode;
    }
}

void init() {
    pin_setup();
    check_inputs();
    // serial_comm_setup();
    // adc_setup();
    display_setup(6);

    preinit_RNG();

    display_update_timer_setup();
    polling_timer_setup();




    


    // enable interrupts
    sei();

}

// TIMER0_OVF_vect


void pin_setup() {
    /* Set Data directions: */
    /* Buttons, switches, encs on PORTD from pins 2~7. */

    DDRD &= 0x03; // 0000 0011 (rx & tx on pins 0 & 1 unchanged)

    PORTD |= 0xFC; // 1111 1100 enable internal pull-up resistors -- "" --

    // buttons

    DDRC |= 0xFF;

    // encoders
    // bitbang-comm pins: check port & set data/clk/cs pins. 

    /* Set internal pull-up resistors for buttons & encoders */

    /* Do anything needed for adc/serial comms?*/
} 

void display_setup(uint8_t brightness) {
    uint8_t numbers[8] = {1, 8, 2, 8, 3, 8, 4, 8};

    maxExitTestMode();
    maxSend8bits(0xFF, SCAN_LIMIT_ADDR);
    maxSetDecodeMode(0);
    maxSetIntensity(brightness);
    maxDisplayNumbers(numbers);
    maxUnShutdown();
}

void adc_setup() {
    // Enable adc, set prescaler, etc. on all free pins. 
}

void serial_comm_setup() {
    // See data sheet. 
}

void preinit_RNG() {
    srand(384729); 
}

void display_update_timer_setup() {
    // TCCR0A = 0x00; // normal operation.
    //TCCR0B = 0x05; // set clock prescaler to 1024
    TCCR0B |= 0x05; // | _BV(CS00);
    // TCNT0 = the timer value register

    OCR0A = 0x80; 

    //TIMSK0 = 0x01; // enable overflow interrupt for timer 0
    TIMSK0 |=  _BV(OCIE0A) | _BV(TOIE0);
}

void cool_visual_effects(uint8_t count) {
    uint8_t i;
    for (i = 0; i < count; i++) {
        maxEnterTestMode();
        _delay_ms(100);
        maxExitTestMode();
        _delay_ms(100);
    }
}

void polling_timer_setup() {
    TCCR2B |= _BV(CS21);
    TIMSK2 |= _BV(TOIE2);
}

void finalize_RNG_init() {

    status.rng_ok = 1;
}
