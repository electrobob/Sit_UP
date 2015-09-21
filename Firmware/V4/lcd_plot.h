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


#ifndef LCD_PLOT_H_
#define LCD_PLOT_H_
#include <avr/pgmspace.h>


#define GR_OFFSET_ADC 200
#define GR_OFFSET_DELTA 120

void lcd_plot_init(void);


#endif /* LCD_PLOT_H_ */