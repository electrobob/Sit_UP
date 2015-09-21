/*
 * SitUp - time management functions
 * Created: 08.06.2015
 *  Author: Bogdan Raducanu
 * copyright (c) Bogdan Raducanu, 2015
 * http://www.electrobob.com/sit-up/
 * bogdan@electrobob.com
 * Released under GPLv3.
 * Please refer to LICENSE file for licensing information.
 */

#include "time.h"
#include <asf.h>

uint32_t time, time_slow, time_now;

void time_update() {
	time = rtc_get_time(); //take proper care of time in the longer 32b counter, normal counter counts till 65536 even though it is 32bits
	time_now = time_slow + time;
	if (time > 32768) {
		time_slow = time_now;
		rtc_set_time(0);
	}
}

uint32_t get_time(void) {
	return time_now;
}
