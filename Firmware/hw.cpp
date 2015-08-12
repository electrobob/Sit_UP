/*
 * SitUp - AUX Hardware functions
 * Created: 08.06.2015
 *  Author: Bogdan Raducanu
 * copyright (c) Bogdan Raducanu, 2015
 * http://www.electrobob.com/sit-up/
 * bogdan@electrobob.com
 * Released under GPLv3.
 * Please refer to LICENSE file for licensing information.
 */

#include <avr/io.h>
#include <util/delay.h>
#include <asf.h>
#include "hw.h"

void buzz_init() {
	ioport_set_pin_dir(BUZZER_PIN, IOPORT_DIR_OUTPUT);

}

void buzz_a_bit(uint16_t buzz_time_ms) {
	for (uint16_t i = 0; i < buzz_time_ms * 2; i++) {
		ioport_set_pin_level(BUZZER_PIN, false);
		_delay_us(250);
		ioport_set_pin_level(BUZZER_PIN, true); //buzzer connected between pin and 3.3V, end in 3.3V
		_delay_us(250);
	}
}

void LED_init(void) {
	ioport_set_pin_dir(LED_PIN, IOPORT_DIR_OUTPUT);
}

void LED_ON(void) {
	ioport_set_pin_level(LED_PIN, true);
}

void LED_blink(uint16_t dur_ms) {
	LED_ON();
	for (uint16_t i = 0; i < dur_ms; i++)
		_delay_ms(1);
	LED_OFF();
}

void LED_OFF(void) {
	ioport_set_pin_level(LED_PIN, false);
}

void vib_init(void) {
	PORTD.DIRSET = 0xB0; //PC7, 5, 4 as outputs
}

void vib_on(uint16_t del) {
	PORTD.OUTSET = 0xB0;
	for (uint16_t i = 0; i < del; i++) {
		_delay_ms(1);
	}
	PORTD.OUTCLR = 0xB0;
}

void vib_off(void) {
	PORTD.OUTCLR = 0xF0;
}

void vib_pulse(uint8_t numbers, uint16_t on_t, uint16_t off_t) {
	for (uint16_t i = 0; i < numbers; i++) {
		vib_on(on_t);
		for (uint16_t j = 0; j < off_t; j++) {
			_delay_ms(1);
		}
	}
}

void vib_15min(void) {
	vib_pulse(4, 150, 250);
}

void vib_1h(void) {
	vib_pulse(10, 250, 500);
}
