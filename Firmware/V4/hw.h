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

#define BUZZER_PIN_P IOPORT_CREATE_PIN(PORTD, 4)
#define BUZZER_PIN_N IOPORT_CREATE_PIN(PORTD, 5)
#define LED_R_PIN IOPORT_CREATE_PIN(PORTD, 1)
#define LED_G_PIN IOPORT_CREATE_PIN(PORTD, 6)
#define LED_B_PIN IOPORT_CREATE_PIN(PORTD, 7)
#define VIB_PIN IOPORT_CREATE_PIN(PORTC, 1)

void buzz_init(void);
void buzz_a_bit(uint16_t buzz_time_ms);

void LED_init(void);
void LED_R_ON(void);
void LED_R_OFF(void);
void LED_G_ON(void);
void LED_G_OFF(void);
void LED_B_ON(void);
void LED_B_OFF(void);
void LED_R_blink(uint16_t dur_ms);
void LED_G_blink(uint16_t dur_ms);
void LED_B_blink(uint16_t dur_ms);

void vib_init(void);
void vib_on(uint16_t del);
void vib_off(void);
void vib_pulse(uint8_t numbers, uint16_t on_t, uint16_t off_t);
void vib_15min(void);
void vib_1h(void);

#endif /* HW_H_ */