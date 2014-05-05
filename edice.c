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

volatile uint8_t die_size = 6;
volatile uint8_t number_of_dice = 1;

uint8_t results[100] = {0};
uint8_t results_len = 1;
uint16_t results_sum = 0;

volatile uint8_t results_scroller_counter = 1;

volatile uint8_t display[8] = {0};

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

        
        // Throw dice, store results, calculate sum. 
        if (status.roll_button_pressed) {            
            results_sum = 0;
            results_len = number_of_dice;
            for (i = 0; i < results_len; i++) {
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
    static uint8_t encoder2_state;
    // button
    status.roll_button_pressed = ~(input_port >> 7);

    // display mode & scroller value
    status.display_mode = (input_port >> 6) & 1;

    // encoder 1
    encoder1_state <<= 2; 
    encoder1_state |= (((input_port >> 1) & 0x02) | ((input_port >> 3) & 0x01) ) & 0x0f;
        
    enc1_count += new_decode_encoder(encoder1_state);
    if (enc1_count > 198) enc1_count = 1;
    else if (enc1_count < 1) enc1_count = 198;

    // Change ndice accordingly. 
    number_of_dice = ((enc1_count >> ENC_COUNTER_TUNE_FACTOR) % MAX_THROWS) + 1;
    if (!status.display_mode) { 
        die_size = ((enc2_count >> ENC_COUNTER_TUNE_FACTOR) % MAX_DIE_SIZE) + 1; 
        if (die_size == 1) die_size = 2;
    }


    // encoder 2 
    encoder2_state <<= 2; 
    encoder2_state |= (((input_port >> 3) & 0x02) | ((input_port >> 5) & 0x01) ) & 0x0f;

    // -> die size or scroller: 
    if (status.display_mode) {
        results_scroller_counter += new_decode_encoder(encoder2_state);

        if (results_scroller_counter > results_len) results_scroller_counter = 1;
        else if (results_scroller_counter == 0) results_scroller_counter = results_len;

    } else {
        enc2_count += new_decode_encoder(encoder2_state);

        if (enc2_count > 198) enc2_count = 1;
        else if (enc2_count < 1) enc2_count = 198;
    }

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


    if (number_of_dice != last_n_dice) {
        maxDisplayFigure(number_of_dice, display, N_OF_DICE_START_DIGIT, 2, status.display_mode); 
        last_n_dice = number_of_dice;
    }
    if (die_size != last_die_size) {
        maxDisplayFigure(die_size, display, DIE_SIZE_START_DIGIT, 2, status.display_mode);
        last_die_size = die_size;
    }

    if (results_changed()) update_results();

    // if (last_results_sum != results_sum) {
    //     update_results();
    //     last_results_sum = results_sum;

    // } else if (status.display_mode != last_display_mode) {
    //     update_results();
    //     last_display_mode = status.display_mode;
    // } else if (results_scroller_counter != last_scroller_value) {
    //     update_results();
    //     last_scroller_value = results_scroller_counter;
    // }

}

uint8_t results_changed() {
    static uint8_t last_results_sum; 
    static uint8_t last_display_mode;
    static uint8_t last_scroller_value;

    if (last_results_sum != results_sum) {
        last_results_sum = results_sum;
        return 1;
    } else if (last_display_mode != status.display_mode) {
        last_display_mode = status.display_mode;
        return 1;
    } else if (last_scroller_value != results_scroller_counter) {
        last_scroller_value = results_scroller_counter;
        return 1;
    }


    return 0;
}

void update_results() {
 
    if (status.display_mode) {
        // TODO: scroller
        if (results_len == 1) {
            maxDisplayFigure(0, display, 5, 2, status.display_mode);
            maxDisplayFigure(results[0], display, 7, 2, status.display_mode);
            results_len = 1;
        } else if (results_len == 2) {
            maxDisplayFigure(results[0], display, 7, 2, status.display_mode);
            maxDisplayFigure(results[1], display, 5, 2, status.display_mode);

        // 3 or more dice: 
        } else {
            if (results_scroller_counter < 10) {
                maxDisplayFigure(results_scroller_counter, display, 5, 1, status.display_mode);
                maxDisplayFigure(0, display, 6, 1, status.display_mode);
            } else {
                maxDisplayFigure(results_scroller_counter, display, 5, 2, status.display_mode);
                maxDisplayFigure(results[results_scroller_counter - 1], display, RESULTS_START_DIGIT + 2, 2, status.display_mode);
            }
        }

    } else {
        maxDisplayFigure(results_sum, display, RESULTS_START_DIGIT, 4, 0);
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
    maxDisplayNumbers(numbers, 1);
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
    // TCCR0B = 0x05; // set clock prescaler to 1024
    // TCCR0B |= 0x05; // | _BV(CS00);
    TCCR0B |= _BV(CS02) | _BV(CS00);
    // TCNT0 = the timer value register

    OCR0A = 0x80; 

    //TIMSK0 = 0x01; // enable overflow interrupt for timer 0
    TIMSK0 |=  _BV(OCIE0A) | _BV(TOIE0);
}

void results_scroller_timer_setup() {
    TIMSK0 |= _BV(OCIE0B);
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
