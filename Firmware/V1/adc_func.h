/*
 * SitUp - ADC functions
 * Created: 08.06.2015
 *  Author: Bogdan Raducanu
 * copyright (c) Bogdan Raducanu, 2015
 * http://www.electrobob.com/sit-up/
 * bogdan@electrobob.com
 * Released under GPLv3.
 * Please refer to LICENSE file for licensing information.
 */

#ifndef ADC_FUNC_H_
#define ADC_FUNC_H_

#define TOUCH_NUM_MEAS 500
#define TOUCH_DELTA_THRESHOLD 10

#define MY_ADC    ADCA
#define CAP_ADC ADC_CH0

#define RATE_OF_CONVERSION       500

#define SENS_PIN    IOPORT_CREATE_PIN(PORTD, 0)
#define LED_ADC_NEG_PIN IOPORT_CREATE_PIN(PORTA, 3)
#define LED_ADC_POS_PIN IOPORT_CREATE_PIN(PORTA, 2)

#define TOUCH_ADC_POS_PIN IOPORT_CREATE_PIN(PORTA, 3)
#define TOUCH_ADC_GND_PIN IOPORT_CREATE_PIN(PORTA, 5)
#define TOUCH_ADC_REF_PIN IOPORT_CREATE_PIN(PORTA, 2)
#define TOUCH_TX_PIN IOPORT_CREATE_PIN(PORTA, 2)

void ADC_init_funct(void);
int read_light_level(void);
int read_cap_level(void);
void ADC_set_input_config(enum adcch_positive_input pos);
int read_cap_level(uint16_t avg_no);
int read_cap_level_ref(uint16_t avg_no);
void capsense_init(void);

#endif /* ADC_FUNC_H_ */
