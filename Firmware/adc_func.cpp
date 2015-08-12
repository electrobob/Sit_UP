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

#include <asf.h>
#include "adc_func.h"
#include <util/delay.h>

void capsense_init(void) {
	ioport_set_pin_dir(SENS_PIN, IOPORT_DIR_INPUT);
	ioport_set_pin_dir(TOUCH_TX_PIN, IOPORT_DIR_OUTPUT);

	PORTA.DIRCLR = 0xFF; //all in
	PORTA.PIN3CTRL = PORT_OPC_TOTEM_gc;

}

void ADC_init_funct(void) {
	struct adc_config adc_conf;
	struct adc_channel_config adcch_conf;

	adc_read_configuration(&MY_ADC, &adc_conf);
	adcch_read_configuration(&MY_ADC, CAP_ADC, &adcch_conf);

	adc_set_conversion_parameters(&adc_conf, ADC_SIGN_OFF, ADC_RES_12,ADC_REF_VCC);
	adc_set_conversion_trigger(&adc_conf, ADC_TRIG_MANUAL, 1, 0);
	adc_set_clock_rate(&adc_conf, 100000);
	adcch_set_input(&adcch_conf, ADCCH_POS_PIN3, ADCCH_NEG_NONE, 1);

	adc_write_configuration(&MY_ADC, &adc_conf);
	adcch_write_configuration(&MY_ADC, CAP_ADC, &adcch_conf);
	adc_enable (&MY_ADC);
}

void ADC_set_input_config(enum adcch_positive_input pos) {
	struct adc_channel_config adcch_conf;
	adcch_read_configuration(&MY_ADC, CAP_ADC, &adcch_conf);
	adcch_set_input(&adcch_conf, pos, ADCCH_NEG_PAD_GND, 1);
	adcch_write_configuration(&MY_ADC, CAP_ADC, &adcch_conf);
}
;

int read_cap_level(uint16_t avg_no) {
	int32_t adcresult = 0;

	for (uint16_t i = 0; i < avg_no; i++) {
		ADC_set_input_config (ADCCH_POS_PIN6); //connect ADC to ADC1 which is shorted to VDD.

		ioport_set_pin_dir(TOUCH_ADC_POS_PIN, IOPORT_DIR_OUTPUT);
		ioport_set_pin_level(TOUCH_ADC_POS_PIN, false); //set measurement pin to GND

		PORTA.PIN0CTRL = PORT_OPC_TOTEM_gc; //totem pole
		ioport_set_pin_dir(TOUCH_ADC_POS_PIN, IOPORT_DIR_INPUT); //input

		ADC_set_input_config (ADCCH_POS_PIN3); //connect ADC to ADC3 which is sensor previously charged to VDD.

		adc_start_conversion(&MY_ADC, CAP_ADC);
		adc_wait_for_interrupt_flag(&MY_ADC, CAP_ADC);
		adcresult += adc_get_result(&MY_ADC, CAP_ADC);
	}
	return adcresult / avg_no;

}

int read_cap_level_ref(uint16_t avg_no) {
	int32_t adcresult = 0;

	for (uint16_t i = 0; i < avg_no; i++) {
		ADC_set_input_config (ADCCH_POS_PIN6); //connect ADC to ADC1 which is shorted to VDD.

		ioport_set_pin_dir(TOUCH_ADC_REF_PIN, IOPORT_DIR_OUTPUT);
		ioport_set_pin_level(TOUCH_ADC_REF_PIN, false); //set measurement pin to GND
		_delay_us(10); //make sure it is discharged

		PORTA.PIN2CTRL = PORT_OPC_TOTEM_gc; //totem pole
		ioport_set_pin_dir(TOUCH_ADC_REF_PIN, IOPORT_DIR_INPUT); //input

		ADC_set_input_config (ADCCH_POS_PIN2); //connect ADC to ADC2 which is not connected to anything
		_delay_us(10);

		adc_start_conversion(&MY_ADC, CAP_ADC);
		adc_wait_for_interrupt_flag(&MY_ADC, CAP_ADC);
		adcresult += adc_get_result(&MY_ADC, CAP_ADC);
	}
	return adcresult / avg_no;

}
