/*
 * SitUp
 * Created: 08.06.2015
 *  Author: Bogdan Raducanu
 * copyright (c) Bogdan Raducanu, 2015
 * http://www.electrobob.com/sit-up/
 * bogdan@electrobob.com
 * Released under GPLv3.
 * Please refer to LICENSE file for licensing information.
 */

#include <avr/pgmspace.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <asf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "adc_func.h"
#include "serial.h"
#include <spi.h>
#include "ili9341.h"
#include "gfx_ili9341_sdt028atft.h"
#include "gfx_text.h"
#include "sysfont.h"
#include "time.h"
#include "hw.h"
#include "esp.h"
#include "lcd_plot.h"

#define MEASURE_INTERVAL 3
#define SLEEP_TIME 3
#define DEBOUNCE_COUNT_ON 2
#define DEBOUNCE_COUNT_OFF 5
#define ALARM_INTERVAL 900 
#define ALARM_CRITICAL 4

//Operation control, as variables for future use
char LCD_enabled = 1;
char UPLOAD_DATA = 1;
char LOW_power = 0;
////////

uart debug(&USARTD0, 115200);
ISR (USARTD0_RXC_vect) {debug.rxInterrupt();}
ISR (USARTD0_DRE_vect) {debug.txInterrupt();}

enum ch_occ {
	not_occupied, occupied
};
ch_occ chair_status = not_occupied;

char out_str[16];
uint32_t now_time, time_last_measure, time_last_loop, time_occupied;
int adc_value, adc_ref_value, delta_cal, adc_delta, adc_value_init,adc_ref_value_init;
uint16_t xpos_plot, ypos_plot, ypos_plot_prev, ydelta, ydelta_old;

extern char csv_data_output[256];
char wifi_connected = 0;
uint16_t global_wifi_reconn = 0;

char update_electrobob = 0;
char person_detected;
uint8_t debounce_cnt = 0;
uint8_t vib_no = 1; //how many times it vibrated

int main(void) {
	//INITIALIZATION
	sysclk_init(); //this SOB turns off clock to all peripherals. Disable that "FEATURE".
	rtc_init();

	PMIC.CTRL = PMIC_HILVLEN_bm | PMIC_MEDLVLEN_bm | PMIC_LOLVLEN_bm; //Enable all interrupts
	PMIC.CTRL |= PMIC_LOLVLEN_bm; // Enable LO interrupt level
	
	sleep_set_mode(SLEEP_SMODE_PSAVE_gc);
	sleep_enable();
	
	sei();
	

	debug.sendString("\nXMEGA SitUp touch\n");

	vib_init();
	capsense_init();
	LED_init();
	buzz_init();

	if (LCD_enabled) { //if LCD is enabled for debug purposes
		lcd_plot_init();
		gfx_ili9341_init();
		gfx_draw_string("Touch test", 0, 0, &sysfont, GFX_COLOR_BLACK,
				GFX_COLOR_WHITE);
	}

	LED_ON();
	ADC_init_funct();
	esp_init();
	buzz_a_bit(1000);
	vib_pulse(3, 100, 100);

	//CALIBRATION
	_delay_ms(10000);
	if (LCD_enabled)
		gfx_draw_string("Calibration", 170, 0, &sysfont, GFX_COLOR_BLACK,
				GFX_COLOR_WHITE);
	_delay_ms(3000); //wait a while after startup
	adc_value_init = read_cap_level(TOUCH_NUM_MEAS);
	adc_ref_value_init = read_cap_level_ref(TOUCH_NUM_MEAS);
	buzz_a_bit(100);
	delta_cal = adc_value_init - adc_ref_value_init;
	if (LCD_enabled)
		gfx_draw_string("Done         ", 170, 0, &sysfont, GFX_COLOR_BLACK,
				GFX_COLOR_WHITE);
	//END Calibration
	LED_OFF();
	
	while (1) {
		time_update(); //get the new time from RTC
		now_time = get_time();
		if (LCD_enabled) {
			itoa(now_time, out_str, 10);
			gfx_draw_string(out_str, 220, 0, &sysfont, GFX_COLOR_BLACK,
					GFX_COLOR_GREEN);
		}

		//read the data everytime going through loop
		adc_value = read_cap_level(TOUCH_NUM_MEAS);
		adc_ref_value = read_cap_level_ref(TOUCH_NUM_MEAS);
		adc_delta = adc_value - adc_ref_value;

		//see if somebody sits
		if (adc_delta < (delta_cal - TOUCH_DELTA_THRESHOLD)) {
			person_detected = 1;
			LED_blink(10);
		} else {
			person_detected = 0;
		}
	
		//see if processing is required and evaluate chair occupancy
		if ((now_time - time_last_measure) >= MEASURE_INTERVAL) { //do a measurement every MEASURE_INTERVAL, be careful about sleep interval
			time_last_measure = now_time;
			debug.sendString("\ntime: ");
			ltoa(now_time, out_str, 10);
			debug.sendString(out_str);

			if (person_detected) {
				if (chair_status == not_occupied) {
					debounce_cnt++;
					if (debounce_cnt >= DEBOUNCE_COUNT_ON) {
						debounce_cnt = 0;
						chair_status = occupied;
						time_occupied = now_time;
						//sprintf(out_str, "\n occupied at %d", time_occupied);
						//debug.sendString(out_str);
						buzz_a_bit(50);
						update_electrobob = 1;
					}
				} else
					debounce_cnt = 0;
			} else {
				if (chair_status == occupied) {
					debounce_cnt++;
					if (debounce_cnt >= DEBOUNCE_COUNT_OFF) {
						debounce_cnt = 0;
						chair_status = not_occupied;
						update_electrobob = 1;
						buzz_a_bit(50);
						_delay_ms(50);
						buzz_a_bit(50);
					}
				} else
					debounce_cnt = 0;
			}

			//ALARM Vibration if chair occupied for too long
			if (chair_status == occupied) {
				if ((now_time - time_occupied) > (vib_no*ALARM_INTERVAL)) {
					vib_no++;
					sprintf(out_str, "\n vibno %d", vib_no);
					debug.sendString(out_str);
					if(vib_no > ALARM_CRITICAL) vib_1h(); else vib_15min(); //at first be nice, then insist
				}
			}

		}

		//UPLOAD data if needed
		if (UPLOAD_DATA) {
			if (update_electrobob) {
				update_electrobob = 0;
				esp_power_on();
				_delay_ms(1000);
				connectWiFi(10);

				if (chair_status == occupied) { //make sure to always upload the transition, 1 to 0 or 0 to 1. 
					sprintf(csv_data_output, "chair_bob:%d}", not_occupied);
					upload_electrobob_data();
					sprintf(csv_data_output, "chair_bob:%d}", occupied);
					upload_electrobob_data();
				} else {
					sprintf(csv_data_output, "chair_bob:%d}", occupied);
					upload_electrobob_data();
					sprintf(csv_data_output, "chair_bob:%d}", not_occupied);
					upload_electrobob_data();
				}
				esp_power_off();
			}
		}

		if (LCD_enabled) {
			//display raw data on LCD
			debug.sendString("\n ADC: ");
			sprintf(out_str, "%04d", adc_value);
			debug.sendString(out_str);
			gfx_draw_string(out_str, 0, 16, &sysfont, GFX_COLOR_BLACK,
					GFX_COLOR_WHITE);

			debug.sendString("\t ref: ");
			sprintf(out_str, "%04d", adc_ref_value);
			debug.sendString(out_str);
			gfx_draw_string(out_str, 50, 16, &sysfont, GFX_COLOR_BLACK,
					GFX_COLOR_WHITE);

			debug.sendString("\t delt: ");
			sprintf(out_str, "%05d", adc_value - adc_ref_value);
			debug.sendString(out_str);
			gfx_draw_string(out_str, 100, 16, &sysfont, GFX_COLOR_BLACK,
					GFX_COLOR_WHITE);

			debug.sendString("\t delt 2: ");
			sprintf(out_str, "%05d", delta_cal - adc_delta);
			debug.sendString(out_str);
			gfx_draw_string(out_str, 160, 16, &sysfont, GFX_COLOR_BLACK,
					GFX_COLOR_YELLOW);

			//display graph
			ypos_plot = adc_value - adc_value_init + GR_OFFSET_ADC;
			gfx_draw_line(xpos_plot, 240, xpos_plot, 28, GFX_COLOR_BLACK);
			gfx_draw_line(xpos_plot, 240 - ypos_plot, xpos_plot,
					240 - ypos_plot_prev, GFX_COLOR_WHITE);
			ypos_plot_prev = ypos_plot;

			ydelta = (adc_value - adc_ref_value) / 2 + GR_OFFSET_DELTA;
			gfx_draw_line(xpos_plot, 240 - ydelta, xpos_plot, 240 - ydelta_old,
					GFX_COLOR_GREEN);
			ydelta_old = ydelta;

			gfx_draw_line(xpos_plot + 1, 28, xpos_plot + 1, 240, GFX_COLOR_RED);
			xpos_plot++;
			if (xpos_plot > 320)
				xpos_plot = 1;
		}
		//Nap time
		rtc_set_alarm_relative(SLEEP_TIME);
		sleep_enter();
	}
}
