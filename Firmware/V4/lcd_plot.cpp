/*
 * SitUp - AUX LCD functions
 * Created: 08.06.2015
 *  Author: Bogdan Raducanu
 * copyright (c) Bogdan Raducanu, 2015
 * http://www.electrobob.com/sit-up/
 * bogdan@electrobob.com
 * Released under GPLv3.
 * Please refer to LICENSE file for licensing information.
 */

#include "lcd_plot.h"
#include <avr/io.h>

void lcd_plot_init(void){
			PORTC.DIRSET = 0xF3; //why does the driver not do this ?????????????
			PORTA.DIRSET = 0x80; //backlight
			PORTA.OUTSET = 0x80;
}