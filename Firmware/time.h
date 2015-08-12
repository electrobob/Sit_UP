/*
 * SitUp - time managemet functions
 * Created: 08.06.2015
 *  Author: Bogdan Raducanu
 * copyright (c) Bogdan Raducanu, 2015
 * http://www.electrobob.com/sit-up/
 * bogdan@electrobob.com
 * Released under GPLv3.
 * Please refer to LICENSE file for licensing information.
 */


#ifndef TIME_H_
#define TIME_H_
#include <avr/pgmspace.h>

void time_update();
uint32_t get_time(void);

#endif /* TIME_H_ */