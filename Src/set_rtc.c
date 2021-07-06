/*
 * set_rtc.c
 *
 *  Created on: Jul 6, 2021
 *      Author: deviousprophet
 */


#include "ds1307.h"

int main() {
	RTC_time_t time;
	time.hours = 14;
	time.minutes = 13;
	time.seconds = 0;
	time.time_format = TIME_FORMAT_24HRS;
	ds1307_init();
	ds1307_set_current_time(&time);
	return 0;
}
