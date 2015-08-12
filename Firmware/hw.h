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


#ifndef HW_H_
#define HW_H_

#define BUZZER_PIN IOPORT_CREATE_PIN(PORTD, 2)
#define LED_PIN IOPORT_CREATE_PIN(PORTD, 6)

void buzz_init(void);
void buzz_a_bit(uint16_t buzz_time_ms);

void LED_init(void);
void LED_ON(void);
void LED_OFF(void);
void LED_blink(uint16_t dur_ms);

void vib_init(void);
void vib_on(uint16_t del);
void vib_off(void);
void vib_pulse(uint8_t numbers, uint16_t on_t, uint16_t off_t);
void vib_15min(void);
void vib_1h(void);

#endif /* HW_H_ */