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
	ioport_set_pin_dir(BUZZER_PIN_P, IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(BUZZER_PIN_N, IOPORT_DIR_OUTPUT);

}

void buzz_a_bit(uint16_t buzz_time_ms) {
	for (uint16_t i = 0; i < buzz_time_ms * 2; i++) {
		ioport_set_pin_level(BUZZER_PIN_N, false);
		ioport_set_pin_level(BUZZER_PIN_P, true);
		_delay_us(250);
		ioport_set_pin_level(BUZZER_PIN_N, true);
		ioport_set_pin_level(BUZZER_PIN_P, false);
		_delay_us(250);
	}
			ioport_set_pin_level(BUZZER_PIN_N, false); //make sure both pins are L at end of buzzing
			ioport_set_pin_level(BUZZER_PIN_P, false);
}

void LED_init(void) {
	ioport_set_pin_dir(LED_R_PIN, IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(LED_G_PIN, IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(LED_B_PIN, IOPORT_DIR_OUTPUT);
}

void LED_R_ON(void) {
	ioport_set_pin_level(LED_R_PIN, true);
}
void LED_R_OFF(void) {
	ioport_set_pin_level(LED_R_PIN, false);
}

void LED_G_ON(void) {
	ioport_set_pin_level(LED_G_PIN, true);
}
void LED_G_OFF(void) {
	ioport_set_pin_level(LED_G_PIN, false);
}
void LED_B_ON(void) {
	ioport_set_pin_level(LED_B_PIN, true);
}
void LED_B_OFF(void) {
	ioport_set_pin_level(LED_B_PIN, false);
}

void LED_R_blink(uint16_t dur_ms) {
	LED_R_ON();
	for (uint16_t i = 0; i < dur_ms; i++)
		_delay_ms(1);
	LED_R_OFF();
}

void LED_G_blink(uint16_t dur_ms) {
	LED_G_ON();
	for (uint16_t i = 0; i < dur_ms; i++)
	_delay_ms(1);
	LED_G_OFF();
}

void LED_B_blink(uint16_t dur_ms) {
	LED_B_ON();
	for (uint16_t i = 0; i < dur_ms; i++)
	_delay_ms(1);
	LED_B_OFF();
}



void vib_init(void) {
	//PORTD.DIRSET = 0xB0; //PC7, 5, 4 as outputs
	ioport_set_pin_dir(VIB_PIN, IOPORT_DIR_OUTPUT);
	ioport_set_pin_level(VIB_PIN, false);
}

void vib_on(uint16_t del) {
	ioport_set_pin_level(VIB_PIN, true);
	for (uint16_t i = 0; i < del; i++) {
		_delay_ms(1);
	}
	ioport_set_pin_level(VIB_PIN, false);
}

void vib_off(void) {
	ioport_set_pin_level(VIB_PIN, false);
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
