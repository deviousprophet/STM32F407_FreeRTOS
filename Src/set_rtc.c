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
	DS1307_DateTime_t datetime;
	char date_buf[11];
	char time_buf[9];

	LCD5110_Init(0x37);

	while(DS1307_Init() != DS1307_Result_OK) {
		LCD5110_Clear();
		LCD5110_Puts("error", 1, 1);
		LCD5110_Refresh();
		for(int i = 0; i < 100000; i++);
	}

	datetime.date = 14;
	datetime.month = 7;
	datetime.year = 21;
	datetime.hours = 16;
	datetime.minutes = 44;
	datetime.seconds = 0;

//	DS1307_SetDateTime(&datetime);

	while(1) {
		DS1307_GetDateTime(&datetime);
		LCD5110_Clear();
		sprintf(date_buf, "%02d/%02d/20%02d",
				datetime.date,
				datetime.month,
				datetime.year);
		LCD5110_Puts(date_buf, 1, 1);

		LCD5110_GotoXY(0, 12);
		sprintf(time_buf, "%02d:%02d:%02d",
				datetime.hours,
				datetime.minutes,
				datetime.seconds);
		LCD5110_Puts(time_buf, 1, 1);
		LCD5110_Refresh();
		for(int i = 0; i < 100000; i++);
	}

	return 0;
}
