/*
 * set_rtc.c
 *
 *  Created on: Jul 6, 2021
 *      Author: deviousprophet
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "ds1307.h"
#include "lcd5110.h"

int main() {
	uint32_t date_value = 130721;
	uint32_t time_value = 124900;

	RTC_date_t date;
	RTC_time_t time;

	date.date = date_value / 10000;
	date.month = (date_value % 10000) / 100;
	date.year = date_value % 100;

	time.hours = time_value / 10000;
	time.minutes = (time_value % 10000) / 100;
	time.seconds = time_value % 100;
	time.time_format = TIME_FORMAT_24HRS;
	ds1307_init();
//	ds1307_set_current_time(&time);
	ds1307_set_current_date(&date);

	char date_buf[11];
	char time_buf[9];

	LCD5110_Init(0x37);

	while(1) {
		ds1307_get_current_time(&time);
		ds1307_get_current_date(&date);
		LCD5110_Clear();
		sprintf(date_buf, "%02d/%02d/20%02d",
				date.date,
				date.month,
				date.year);
		LCD5110_Puts(date_buf, 1, 1);

		LCD5110_GotoXY(0, 12);
		sprintf(time_buf, "%02d:%02d:%02d",
				time.hours,
				time.minutes,
				time.seconds);
		LCD5110_Puts(time_buf, 1, 1);
		LCD5110_Refresh();
		for(int i = 0; i < 100000; i++);
	}

	return 0;
}
