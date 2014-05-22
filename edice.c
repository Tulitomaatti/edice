// edice test/main file

#include "edice.h"

#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>

#include <stdlib.h>

#include "max7221.h"
#include "encoders.h"
#include "serial.h"
#include "random.h"



/* Debug magic variables & functions */
extern uint8_t _end;
extern uint8_t __stack;
uint16_t stackcount;


void StackPaint(void) __attribute__ ((naked)) __attribute__ ((section (".init1")));

void StackPaint(void)
{
	#if 0
	uint8_t *p = &_end;

	while(p <= &__stack)
	{
		*p = STACK_CANARY;
		p++;
	}
	#else
	__asm volatile ("    ldi r30,lo8(_end)\n"
	"    ldi r31,hi8(_end)\n"
	"    ldi r24,lo8(0xc5)\n" /* STACK_CANARY = 0xc5 */
	"    ldi r25,hi8(__stack)\n"
	"    rjmp .cmp\n"
	".loop:\n"
	"    st Z+,r24\n"
	".cmp:\n"
	"    cpi r30,lo8(__stack)\n"
	"    cpc r31,r25\n"
	"    brlo .loop\n"
	"    breq .loop"::);
	#endif
}

uint16_t StackCount(void)
{
	const uint8_t *p = &_end;
	uint16_t       c = 0;

	while(*p == 0xc5 && p <= &__stack)
	{
		p++;
		c++;
	}

	return c;
}


/* Global variables */
uint32_t seed;
uint16_t timer1_overflow_counter = 0;
uint16_t adc_result;
uint8_t luck_counter = 0;
uint32_t luck_timout_counter = 0;
uint8_t lucky_number_visited = 0; 
uint8_t luck_expiry_counter = 0;

volatile uint8_t enc1_count = 2;
volatile uint8_t enc2_count = 10;

volatile uint8_t encoder1_state;
volatile uint8_t encoder2_state;

volatile uint8_t die_size = 6;
volatile uint8_t number_of_dice = 1;
volatile uint8_t results_scroller = 1;
volatile uint8_t brightness_global = 0x0A;

volatile uint8_t results[100] = {0};
volatile uint8_t results_len = 1;
volatile uint16_t results_sum = 0;

volatile uint8_t results_scroller_counter = 1;


// adding this will create "funky" effects (if halved i length.)
// const char huge_array[] = "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789";

volatile struct {
	uint8_t rng_ok : 1;
	uint8_t display_mode : 1;
	uint8_t roll_button_pressed : 1;
	uint8_t luck : 1; //  1 for good luck, 0 for normal operation.
	uint8_t brightness : 4;
} status;

int main(void) {
	uint8_t pressing = 0;
	uint8_t count = 0;
	uint8_t thrown = 0;
	uint16_t max_result;
	uint32_t temp;
	uint8_t i;
	init();

// 
// 	if (RANDOMNESS_TESTING_MODE) {
// 		cli();
// 		finalize_RNG_init();
// 		
// 		maxDisplayFigure(1337, 1, 8, 1, 0);
// 		
// 		while (1){
// 			
// 			cli();
// 			die_size = 2;
// 			number_of_dice = 1;
// 			
// 			throw_dice();
// 			USART_Transmit(results[0]);
// 			
// 	/*		i++;*/
// 			//USART_Transmit(random_uint8());
// 			//temp = random_32int();
// // 			temp = random();
// // 	 		USART_Transmit(temp >> 24);
// // 	 		USART_Transmit(temp >> 16);
// // 			USART_Transmit(temp >> 8);
// // 			USART_Transmit(temp);
// 		}
// 	}

	// looping logic:
	while (1) {
		
		cli();
		
		if (RANDOMNESS_TESTING_MODE) {
			
			
			if (!status.rng_ok) {
				finalize_RNG_init();
			}
			pressing = 1;
			
			max_result = die_size * number_of_dice;
			if (max_result < 10) count = 1;
			else if (max_result < 100) count = 2;
			else if (max_result < 1000) count = 3;
			else count = 4;
			
			throw_dice();
			
			if (status.display_mode) USART_Transmit(results[i]);
			else USART_Transmit(results_sum);
			
			}
		
		
		
		if (status.roll_button_pressed) {
			if (!status.rng_ok) {
				finalize_RNG_init();
				set_sleep_mode(SLEEP_MODE_PWR_SAVE);
			}
			pressing = 1;
			
			max_result = die_size * number_of_dice;
			if (max_result < 10) count = 1;
			else if (max_result < 100) count = 2;
			else if (max_result < 1000) count = 3; 
			else count = 4;
			
			if (!thrown) {
				throw_dice();
				thrown = 1;
			}
			maxDisplayFigure(0,RESULTS_START_DIGIT,3,0,1);
			if (number_of_dice == 2) cool_visual_effects(4);
			else if (number_of_dice == 3) cool_visual_effects(3);
			else cool_visual_effects(count);

			
		} else {
			if (pressing) {
				update_results();
				thrown = 0;
				pressing = 0;
			} 
		}
		
		sei();		
		_delay_us(10);
		
		// go to sleep until interrupted. 
		if(!RANDOMNESS_TESTING_MODE) {	
		cli();
		sleep_enable();
		sleep_bod_disable();
		sei();
		sleep_cpu();
		sleep_disable();
		
		cli();
		}
		check_inputs();
		sei();
	}

	return 0;	
}


//update displays on every overflow (and halfway up)
ISR(TIMER0_OVF_vect) { // Display updating timer
	if (!status.roll_button_pressed) update_displays();
}

ISR(TIMER0_COMPA_vect) { // Display updating timer
	if (!status.roll_button_pressed) update_displays();
}

ISR(TIMER1_OVF_vect) { // RNG seeding timer
	timer1_overflow_counter++;
}

ISR(TIMER2_OVF_vect) { // Input checking timer
	static uint8_t last_display_mode = 3;
	if (status.luck) {
		luck_expiry_counter--;
		if (luck_expiry_counter == 0) {
			status.luck = 0;
			//maxExitTestMode();
		}
	} else {
	luck_timout_counter++; 
	if (luck_timout_counter > 0x0AFF) {
		luck_timout_counter = 0;
		luck_counter = 0;
	}
	
	if (last_display_mode != status.display_mode) {
		luck_counter++;
		last_display_mode = status.display_mode;
	}
	
	
	if (lucky_number_visited && luck_counter > 4) {
		status.luck = 1;
		luck_expiry_counter = 0x0F;
		//maxEnterTestMode();
	} 
	}	
	// input is checked in main, this just wakes cpu from sleep. 
}

void check_inputs() {
	TCCR2B &= ~(_BV(CS20) | _BV(CS21) | _BV(CS22)); //disable input polling timer, just to be sure.

	uint8_t input_port = PIND;
	uint8_t aux = 0;

	static uint8_t scroller_counter;

	// button
	status.roll_button_pressed = ~(input_port >> PIND7);

	// display mode & scroller value
	status.display_mode = (input_port >> PIND6) & 1;

	// encoder 1
	encoder1_state <<= 2;
	encoder1_state |= (((input_port >> (PIND2 - 1) ) & 0x02) | ((input_port >> PIND3) & 0x01) ) & 0x0f;
	
	enc1_count += new_decode_encoder(encoder1_state);
	if (enc1_count > 201) enc1_count = 1;
	else if (enc1_count < 1) enc1_count = 201;

	// Change ndice accordingly.
	number_of_dice = ((enc1_count >> ENC_COUNTER_TUNE_FACTOR) % MAX_THROWS) + 1;


	// encoder 2
	encoder2_state <<= 2;
	encoder2_state |= (((input_port >> (PIND4 - 1)) & 0x02) | ((input_port >> PIND5) & 0x01) ) & 0x0f;
	aux += new_decode_encoder(encoder2_state);
	
	// -> die size or scroller:
	
	if (status.display_mode) {
		if (number_of_dice < 3) {
			status.brightness += aux;
		} else {
			scroller_counter += aux;
			
			if (scroller_counter > 201) scroller_counter = 1;
			else if (scroller_counter < 1) scroller_counter = 201;
			
			results_scroller_counter = ((scroller_counter >> ENC_COUNTER_TUNE_FACTOR) % results_len) + 1;
			
			if (results_scroller_counter > results_len) results_scroller_counter = 1;
			else if (results_scroller_counter == 0) results_scroller_counter = results_len;
		}
		
	} else {
		enc2_count += aux;

		if (enc2_count > 201) enc2_count = 1;
		else if (enc2_count < 1) enc2_count = 201;
	}
	die_size = ((enc2_count >> ENC_COUNTER_TUNE_FACTOR) % MAX_DIE_SIZE) + 1;
	if (die_size < MIN_DIE_SIZE) die_size = MIN_DIE_SIZE;


	if (number_of_dice == 13 && die_size == 37) lucky_number_visited = 1; 


	polling_timer_setup();
}

void update_displays() {
	// remember last display state to avoid futile updating.
	static uint8_t last_n_dice;
	static uint8_t last_die_size;
	static uint8_t last_brightness;
	
	
	// if we have 100 don't use leading zero suppression to get 00 on the screen. 
	if (number_of_dice != last_n_dice || last_die_size != die_size) {
		if (number_of_dice == 100) {
			maxDisplayFigure(number_of_dice, N_OF_DICE_START_DIGIT, 2, 0, 0);
			last_n_dice = number_of_dice;
		} else {
			maxDisplayFigure(number_of_dice, N_OF_DICE_START_DIGIT, 2, 0, 1);
			last_n_dice = number_of_dice;
		}
	}
	if (die_size != last_die_size) {
		if (die_size == 100) {
			maxDisplayFigure(die_size, DIE_SIZE_START_DIGIT, 2, 0, 0);
			last_die_size = die_size;
		} else {
			maxDisplayFigure(die_size, DIE_SIZE_START_DIGIT, 2, 0, 1);
			last_die_size = die_size;
		}
	}
	
	
	if (last_brightness != status.brightness) {
		maxSetIntensity(status.brightness | 0xF0);
		last_brightness = status.brightness;
	}

	if (results_changed()) update_results();
}

void throw_dice() {
	uint8_t i, temp;
	results_sum = 0;
	results_len = number_of_dice;
	for (i = 0; i < results_len; i++) {
		// results_sum += results[i] = ( ((random_32int() ^ TCNT1)  ) % die_size) + 1; // TODO: Fix: % die_size will slant the distribution.
		//results_sum += results[i] =  (random_uint8()   % die_size) + 1; // TODO: Fix: % die_size will slant the distribution.
 	//	temp = random_uint8();
// 		temp ^= TCNT1;
// 	//	temp ^= get_analog_noise_8(1);
// 		
		
 		results[i] = scale_random_uint8_range(die_size); 
		
		if (status.luck && results[i] < die_size / 2) { // scale results if we are in a lucky mood. 
			temp = scale_random_uint8_range(die_size);
			if (temp > results[i]) results[i] = temp;
		} 
		 
		results_sum += results[i];

	}
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
	uint8_t i;
	
	if (status.display_mode) {
		// TODO: brightness control in 2nd mode

		// with 1 & 2 dice, always show decimal point to remind user that we are in that display mode.
		if (results_len == 1) {
				maxDisplayFigure(0, RESULTS_START_DIGIT, 4, 1, 1);
				maxDisplayFigure(results[0], RESULTS_START_DIGIT + 1, 3, 1, 1);
		} else if (results_len == 2) {
			for (i = 0; i < results_len; i++) {
				if (results[i] == 100) maxDisplayFigure(results[i], RESULTS_START_DIGIT + 2*i, 2, 1, 0);
				else maxDisplayFigure(results[i], RESULTS_START_DIGIT + 2*i, 2, 1, 1);
			}

		} else if ( die_size < 11 && results_len <= 4) {
			if (results_len == 3) {
				maxDisplayFigure(0, RESULTS_START_DIGIT, 1, 0, 1);
				for (i = 0; i < results_len; i++) {
					if (results[i] == 10) maxDisplayFigure(results[i], RESULTS_START_DIGIT + 1 + i, 1, 1, 0);
					else maxDisplayFigure(results[i], RESULTS_START_DIGIT + 1 + i, 1, 1, 1);
				}
			} else {
				for (i = 0; i < results_len; i++) {
					if (results[i] == 10) maxDisplayFigure(results[i], RESULTS_START_DIGIT + i, 1, 1, 0);
					else maxDisplayFigure(results[i], RESULTS_START_DIGIT + i, 1, 1, 1);
				}
			}

			
// 			if (results_len == 3) {
// 				maxDisplayFigure(0, RESULTS_START_DIGIT, 1, 0, 1);
// 				maxDisplayFigure(results[0], RESULTS_START_DIGIT + 1, 1, 1, 1);
// 				maxDisplayFigure(results[1], RESULTS_START_DIGIT + 2, 1, 1, 1);
// 				maxDisplayFigure(results[2], RESULTS_START_DIGIT + 3, 1, 1, 1);
// 			} else {
// 				maxDisplayFigure(results[0], RESULTS_START_DIGIT + 0, 1, 1, 1);
// 				maxDisplayFigure(results[1], RESULTS_START_DIGIT + 1, 1, 1, 1);
// 				maxDisplayFigure(results[2], RESULTS_START_DIGIT + 2, 1, 1, 1);
// 				maxDisplayFigure(results[3], RESULTS_START_DIGIT + 3, 1, 1, 1);
// 			}

			// 3 or more dice where size > 9:
		} else {
			if (results_scroller_counter < 10) {
				maxDisplayFigure(results_scroller_counter, RESULTS_START_DIGIT, 1, 1, 1);
				maxDisplayFigure(0, RESULTS_START_DIGIT + 1, 1, 0, 1);
				
				if (results[results_scroller_counter - 1] == 100)  maxDisplayFigure(results[results_scroller_counter - 1], RESULTS_START_DIGIT + 1, 3, 0, 1);
				else maxDisplayFigure(results[results_scroller_counter - 1], RESULTS_START_DIGIT + 2, 2, 0, 1);
			} else {
				if (results_scroller_counter == 100) maxDisplayFigure(results_scroller_counter, RESULTS_START_DIGIT, 2, 1, 0);
				else maxDisplayFigure(results_scroller_counter, RESULTS_START_DIGIT, 2, 1, 1);
				
				if (results[results_scroller_counter - 1] == 100)  maxDisplayFigure(results[results_scroller_counter - 1], RESULTS_START_DIGIT + 2, 2, 0, 0);
				else maxDisplayFigure(results[results_scroller_counter - 1], RESULTS_START_DIGIT + 2, 2, 0, 1);
			}
		}

	} else {
		if (results_sum == 10000) {
			maxDisplayFigure(0, RESULTS_START_DIGIT, 4, 0, 0);
			} else {
			maxDisplayFigure(results_sum, RESULTS_START_DIGIT, 4, 0, 1);
		}

	}
}

void init() {
	uint8_t aux;
	rng_timer_setup();
	display_update_timer_setup();
	polling_timer_setup();
	
	pin_setup();
	
	// update encoder states. 
	aux = PIND;
	encoder1_state |= (((aux >> (PIND2 - 1)) & 0x02) | ((aux >> PIND3) & 0x01) ) & 0x0f;
	encoder2_state |= (((aux >> (PIND4 - 1)) & 0x02) | ((aux >> PIND5) & 0x01) ) & 0x0f;
	encoder1_state <<= 2;
	encoder2_state <<= 2;
	encoder1_state |= (((aux >> (PIND2 - 1)) & 0x02) | ((aux >> PIND3) & 0x01) ) & 0x0f;
	encoder2_state |= (((aux >> (PIND4 - 1)) & 0x02) | ((aux >> PIND5) & 0x01) ) & 0x0f;
	number_of_dice = 2;
	die_size = 6;
	
	check_inputs();

	cli();
	serial_comm_setup();
	display_setup(0x0D);
	
	preinit_RNG();
	status.luck = 0;
	
	set_sleep_mode(SLEEP_MODE_IDLE);
	sei();
}




void preinit_RNG() {
	// TODO: use a seed sampled from analogue noise.
	uint32_t noise;
	uint8_t i;
	seed = 0xdeadbeef;
	seed ^= TCNT1;
	
	noise = timer1_overflow_counter;
	seed ^= noise << 16;
	
	noise = TCNT0; 
	seed ^= noise << 24;
	
	noise = TCNT2;
	seed ^= noise << 16;
	
	// sample adc noice
	seed ^= get_analog_noise(2);
	
	// wait for a adc noise amount
	
	

	// get new noise
	seed ^= get_analog_noise(1);

	noise ^= TCNT1;
	seed ^= noise << 16;
	
// 	USART_Transmit(0xBE);
// 	USART_Transmit(0xEF);
// 	USART_Transmit(0xFA);
// 	USART_Transmit(0xCE);
// 	USART_Transmit(seed >> 24);
// 	USART_Transmit(seed >> 16);
// 	USART_Transmit(seed >> 8);
// 	USART_Transmit(seed);
// 	
// 	init_tinymt(seed);

}

void finalize_RNG_init() {
	// TODO: read 16 bit timer and more adc.
	
	volatile uint32_t noise;
	uint8_t i;
	seed = 0xdeadbeef;
	seed ^= TCNT1;
		
	noise = timer1_overflow_counter;
	seed ^= noise << 16;
		
	noise = TCNT0;
	seed ^= noise << 24;
		
	noise = TCNT2;
	seed ^= noise << 16;
		
	// sample adc noice
	
	for (i = 0; i < 8; i++) {
		noise = get_analog_noise(4);
		seed ^= noise;
// 		
// 		USART_Transmit(0xFF);
// 		USART_Transmit(0xAA);
// 		
// 		USART_Transmit(noise >> 24);
// 		USART_Transmit(noise >> 16);
// 		USART_Transmit(noise >> 8);
// 		USART_Transmit(noise);

	}
	// wait for a adc noise amount
		
	// get new noise
		


	USART_Transmit(seed >> 24);
	USART_Transmit(seed >> 16);
	USART_Transmit(seed >> 8);
	USART_Transmit(seed);
		
	init_tinymt(seed);
	srandom(seed);

	
	status.rng_ok = 1;
}

void display_update_timer_setup() {
	// Set timer 0 prescaler to 1024 (max)
	TCCR0B |= _BV(CS02) | _BV(CS00);        // TCNT0 = the timer value register

	// Timer 0A compare at midway to 0xFF.
	OCR0A = 0x80;

	// enable overflow and compare a interrupt for timer 0
	TIMSK0 |=  _BV(OCIE0A) | _BV(TOIE0);
}

void rng_timer_setup() {
	//TODO: Enable 16bit timer, make a function to get cycles.
	TCCR1B |= _BV(CS01);
	TIMSK1 |= _BV(TOIE1);
}


void polling_timer_setup() {
	// Set prescaler and enable timer 2 overflow interrupt.
	TCCR2B |= _BV(CS21);
	TIMSK2 |= _BV(TOIE2);
}


void pin_setup() {
	/* Set Data directions: */
	/* Buttons, switches, encs on PORTD from pins 2~7. */

	DDRD &= 0x03; // 0000 0011 (rx & tx on pins 0 & 1 unchanged)
	PORTD |= 0xFC; // 1111 1100 enable internal pull-up resistors -- "" --

// 	// Set port C to output for bitbang communications to max7129
// 	DDRC = 0x00;
// 	DDRC |= _BV(PORTC0)|_BV(PORTC1)|_BV(PORTC2); // 
// 	//DDRB = 0xFF;
// 	PORTC &= ~(_BV(PORTC0)|_BV(PORTC1)|_BV(PORTC2));
	
	
	// Set port B to output for bitbang communications to max7129
	BITBANG_DDR = 0x00;
	BITBANG_DDR |= _BV(DATA_PIN)|_BV(CS_PIN)|_BV(CLK_PIN); //
	// BITBANG_DDR = 0xFF; 
	

	BITBANG_PORT &= ~(_BV(DATA_PIN)|_BV(CS_PIN)|_BV(CLK_PIN));

	DDRC = 0x00;
	PORTC = 0x00;
	

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

void read_adc(uint8_t pin) {
	// TODO: temperature
	// admux,	mux3..0 = 1000 => temperature sensor.
	// requires 1.1v internal reference selection

	// slow adc down a bit, set avref to vcc clear mux bits. 
	ADCSRA |= (1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0)|(1<<ADEN);
	ADMUX |= _BV(REFS0);
	ADMUX &= 0xF0; // mux bits are the lower nibble


	// pins 0 1 2 3 4 5 are normal pins. 
	if (pin == 8) { 
		adc_result = 0xc0de; // so that nothing weird happens if we try to read temperature with wrong reference voltage. 
	} else {
		ADMUX |= 0x0F & pin; 
		ADCSRA |= _BV(ADEN); // enable adc
		ADCSRA |= _BV(ADSC); // start adconversion
	
		while (ADCSRA & _BV(ADSC));

		adc_result = ADC;
	}
}

void serial_comm_setup() {
	USART_Init(MYUBRR);
	// See data sheet and serial.h.
}

uint8_t get_analog_noise_8(uint8_t passes) {
	static uint8_t noise = 0xAA;
	uint8_t temp;
	uint8_t i, j, k;
	
	for (k = 0; k < passes; k++) {
		for (i = 0; i < 6; i++) {
			read_adc(i);
			temp = adc_result;
			temp <<= 4;
			
			i++;
			
			read_adc(i);
			temp = adc_result;
			
			noise ^= temp;
		}
	}

// 	for (k = 0; k < passes; k++) { // do it more. 
// 		for (i = 0; i < 6; i++) { // sample once from each adc pin
// 			for (j = 0; j < 8; j++) { // roll around 8 times
// 				read_adc(i);
// 				temp = adc_result;
// 				temp <<= j;
// 				noise ^= temp;
// 			}
// 		}
// 	}
// 	
	return noise;
}




uint32_t get_analog_noise(uint8_t passes) {
	static uint32_t noise = 87063037;
	uint32_t temp;
	uint8_t i, j, k;

	for (k = 0; k < passes; k++) {
		for (i = 0; i < 6; i++) {
			for (j = 0; j < 32; j++) {
				read_adc(i);
				temp = adc_result;
				temp <<= j;
				noise ^= temp;
			}
		}
	}
		
	return noise;	
}
	