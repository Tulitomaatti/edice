// edice test/main file 

#include "edice.h"


#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include <stdlib.h>

#include "max7221.h"
#include "encoders.h"
#include "serial.h"
#include "random.h"


/* Global variables */
volatile uint8_t enc1_count = 2;
volatile uint8_t enc2_count = 12; 

volatile uint8_t die_size = 6;
volatile uint8_t number_of_dice = 1;

volatile uint8_t results[100] = {0};
volatile uint8_t results_len = 1;
volatile uint16_t results_sum = 0;

volatile uint8_t results_scroller_counter = 1;

char pad[] = "\x11";

volatile struct {
    uint8_t rng_ok : 1;
    uint8_t display_mode : 1;
    uint8_t roll_button_pressed : 1;
    int8_t luck : 2; // -1 for bad luck, 0 for off, 1 for good luck.  
    uint8_t something : 3;
} status; 

int main(void) {
   uint8_t i; 

    
    init();

    if (!status.rng_ok) {
        finalize_RNG_init();
    }


    // while(1) {

    //     cli();

    //     if (status.roll_button_pressed) {
    //         throw_dice();
    //         update_displays();    
    //     }

    //     sei();

    //     _delay_ms(5);
    // }


    // looping logic: 
    while (1) {
        cli();

        if (RANDOMNESS_TESTING_MODE) {
            USART_Transmit(random_32int());

            //USART_Transmit(0x55);
            continue;
        } else {
        
        // Throw dice, store results, calculate sum. 
            if (status.roll_button_pressed) {            
                results_sum = 0;
                results_len = number_of_dice;
                for (i = 0; i < results_len; i++) {
                    results_sum += results[i] = (random_32int() % die_size) + 1; // TODO: Fix: % die_size will slant the distribution. 
                }
            }
        }


        
        
        // USART_Transmit('\r');

        sei();  
        _delay_ms(1);

    }

    // TODO: Go to power saving mode, wait for an timer interrupt.
    return 0;
}




void check_inputs() {
    TCCR2B &= ~(_BV(CS20) | _BV(CS21) | _BV(CS22)); //disable input polling timer, just to be sure.

    uint8_t input_port = PIND;

    static uint8_t encoder1_state;
    static uint8_t encoder2_state;
    // button
    status.roll_button_pressed = ~(input_port >> PIND7);

    // display mode & scroller value
    status.display_mode = (input_port >> PIND6) & 1;

    // encoder 1
    encoder1_state <<= 2; 
    encoder1_state |= (((input_port >> (PIND2 - 1) ) & 0x02) | ((input_port >> PIND3) & 0x01) ) & 0x0f;
        
    enc1_count += new_decode_encoder(encoder1_state);
    if (enc1_count > 198) enc1_count = 1;
    else if (enc1_count < 1) enc1_count = 198;

    // Change ndice accordingly. 
    number_of_dice = ((enc1_count >> ENC_COUNTER_TUNE_FACTOR) % MAX_THROWS) + 1;

    if (!status.display_mode) { 
        die_size = ((enc2_count >> ENC_COUNTER_TUNE_FACTOR) % MAX_DIE_SIZE) + 1; 
        if (die_size < MIN_DIE_SIZE) die_size = MIN_DIE_SIZE;
    }


    // encoder 2 
    encoder2_state <<= 2; 
    encoder2_state |= (((input_port >> (PIND4 - 1)) & 0x02) | ((input_port >> PIND5) & 0x01) ) & 0x0f;

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


//update displays on every overflow (and halfway up)
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
        maxDisplayFigure(number_of_dice, N_OF_DICE_START_DIGIT, 2, 0); 
        last_n_dice = number_of_dice;
    }
    if (die_size != last_die_size) {
        maxDisplayFigure(die_size, DIE_SIZE_START_DIGIT, 2, 0);
        last_die_size = die_size;
    }

    if (results_changed()) update_results();
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
        // TODO: brightness control in 2nd mode

        // with 1 & 2 dice, always show decimal point to remind user that we are in that display mode. 
        if (results_len == 1) {
            maxDisplayFigure(0, RESULTS_START_DIGIT, 4, 1);
            maxDisplayFigure(results[0], RESULTS_START_DIGIT + 2, 2, 1);
            results_len = 1;
        } else if (results_len == 2) {
            maxDisplayFigure(results[0], RESULTS_START_DIGIT + 2, 2, 1);
            maxDisplayFigure(results[1], RESULTS_START_DIGIT, 2, 1);

        } else if ( die_size < 10 && results_len <= 4) {
            if (results_len == 3) {
                maxDisplayFigure(0, RESULTS_START_DIGIT, 1, 0);
                maxDisplayFigure(results[0], RESULTS_START_DIGIT + 1, 1, 1);
                maxDisplayFigure(results[1], RESULTS_START_DIGIT + 2, 1, 1);
                maxDisplayFigure(results[2], RESULTS_START_DIGIT + 3, 1, 1);
            } else {
                maxDisplayFigure(results[0], RESULTS_START_DIGIT + 0, 1, 1);
                maxDisplayFigure(results[1], RESULTS_START_DIGIT + 1, 1, 1);
                maxDisplayFigure(results[2], RESULTS_START_DIGIT + 2, 1, 1);
                maxDisplayFigure(results[3], RESULTS_START_DIGIT + 3, 1, 1);
            }

        // 3 or more dice where size > 9: 
        } else {
            if (results_scroller_counter < 10) {
                maxDisplayFigure(results_scroller_counter, RESULTS_START_DIGIT, 1, 1);
                maxDisplayFigure(0, RESULTS_START_DIGIT + 1, 1, 0);
                maxDisplayFigure(results[results_scroller_counter - 1], RESULTS_START_DIGIT + 2, 2, 0);
            } else {
                maxDisplayFigure(results_scroller_counter, RESULTS_START_DIGIT, 2, 1);
                maxDisplayFigure(results[results_scroller_counter - 1], RESULTS_START_DIGIT + 2, 2, 0);
            }
        }

    } else {
        maxDisplayFigure(results_sum, RESULTS_START_DIGIT, 4, 0);
    }
}

void init() {
    pin_setup();
    check_inputs();


    cli();

    serial_comm_setup();
        USART_Transmit(0x12);

    
    //adc_setup();


    display_setup(0xFF);


    preinit_RNG();


    display_update_timer_setup();
    polling_timer_setup();

    // enable interrupts
    sei();
}


void pin_setup() {
    /* Set Data directions: */
    /* Buttons, switches, encs on PORTD from pins 2~7. */

    DDRD &= 0x03; // 0000 0011 (rx & tx on pins 0 & 1 unchanged)
    PORTD |= 0xFC; // 1111 1100 enable internal pull-up resistors -- "" --

    // Set port C to output for bitbang communications to max7129
    DDRC |= 0xFF;
    //DDRB = 0xFF;

    /* Do anything needed for adc/serial comms?*/
    // TODO: enable some ADC pins & adc conversion
    // pins: here, adc: in adc init function.
} 

void display_setup(uint8_t brightness) {
    uint8_t numbers[8] = {1, 8, 2, 8, 3, 8, 4, 8}; // some random numbers for testing. 

    maxExitTestMode();
    maxSend8bits(0xFF, SCAN_LIMIT_ADDR);
    maxSetDecodeMode(0);
    maxSetIntensity(brightness);
    maxDisplayNumbers(numbers);
    maxUnShutdown();
}

void adc_setup() {
    // Enable adc, set prescaler, etc. on all free pins. 
    // TODO
}

void serial_comm_setup() {
    USART_Init(MYUBRR);
    // See data sheet and serial.h. 
}

void preinit_RNG() {
    // TODO: use a seed sampled from analogue noise.
    init_tinymt(9837543); 

}

void display_update_timer_setup() {
    // Set timer 0 prescaler to 1024 (max) 
    TCCR0B |= _BV(CS02) | _BV(CS00);        // TCNT0 = the timer value register

    // Timer 0A compare at midway to 0xFF. 
    OCR0A = 0x80; 

    // enable overflow and compare a interrupt for timer 0
    TIMSK0 |=  _BV(OCIE0A) | _BV(TOIE0);
}

void rng_seeding_timer_setup() {
    //TODO: Enable 16bit timer, make a function to get cycles.
}


// this might not be used anywhere? I can't remember why I wrote this function:
void results_scroller_timer_setup() {
    TIMSK0 |= _BV(OCIE0B);
}


//not used for now. blinks. 
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
    // Set prescaler and enable timer 2 overflow interrupt.
    TCCR2B |= _BV(CS21);
    TIMSK2 |= _BV(TOIE2);
}



void finalize_RNG_init() {
    // TODO: read 16 bit timer and more adc. 
    status.rng_ok = 1;
}


