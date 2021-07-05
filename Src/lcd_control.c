/*
 * lcd_control.c
 *
 *  Created on: Jun 13, 2021
 *      Author: deviousprophet
 */


#include "lcd_control.h"

#define ROW_LINE_1				0
#define ROW_LINE_2				12
#define ROW_LINE_3_1			24
#define ROW_LINE_3_2			29
#define ROW_LINE_4				41

#define VALUE_COLUMN			28
#define VALUE_RANGE_COLUMN		60
#define UNITS_COLUMN			66

typedef struct {
	uint32_t days;
	uint8_t hours;
	uint8_t minutes;
	uint8_t seconds;
} Screen_Timer_t;


LCD_Screen3_Mode screen3_mode;
Screen_Timer_t Screen3_Timer;


enum {
	Screen4_Config_pkv,
	Screen4_Config_pki,
	Screen4_Config_sag,
	Screen4_Config_interval,
	Screen4_Config_date,
	Screen4_Config_time,
	Screen4_No_of_Config_items
} Screen4_Config_Target;

typedef enum {
	Sample_Interval_5 = 5,
	Sample_Interval_15 = 15,
	Sample_Interval_30 = 30,
	Sample_Interval_60 = 60
} Sample_Interval_t;

LCD_Screen4_Mode screen4_mode;

Sample_Interval_t Sample_Interval, Sample_Interval_Config;
LCD_Screen4_RTC_t Screen4_RTC, Screen4_RTC_Config;

LCD_Data_Screen1_t screen1_data;
LCD_Data_Screen2_t screen2_data;
LCD_Data_Screen3_t screen3_data;
LCD_Data_Screen4_t screen4_data, screen4_data_config;

void lcd_puts_xy(unsigned char x, unsigned char y, char* c, LCD5110_Pixel_t color, LCD5110_FontSize_t font) {
	LCD5110_GotoXY(x, y);
	LCD5110_Puts(c, color, font);
}

void range_scale(float value, char* value_buf, char* range) {
	if(value < 1000) {
		sprintf(range, "%s", "m");
	} else if(value < 1000*1000) {
		sprintf(range, "%s", " ");
		value /= 1000;
	} else if(value < 1000*1000*1000) {
		sprintf(range, "%s", "k");
		value /= 1000*1000;
	}

	if(value < 10)
		sprintf(value_buf, "%1.3f", value);
	else if(value < 100)
		sprintf(value_buf, "%2.2f", value);
	else if(value < 1000)
		sprintf(value_buf, "%3.1f", value);
}

void lcd_screen_1_refresh() {
	char val_buf[10];
	char range_buf[2];
	LCD5110_Clear();

	lcd_puts_xy(0, ROW_LINE_1, "Vrms", 1, 1);
	range_scale(screen1_data.Vrms, val_buf, range_buf);
	lcd_puts_xy(VALUE_COLUMN, ROW_LINE_1, val_buf, 1, 1);
	lcd_puts_xy(VALUE_RANGE_COLUMN, ROW_LINE_1, range_buf, 1, 1);
	lcd_puts_xy(UNITS_COLUMN, ROW_LINE_1, "V", 1, 1);

	lcd_puts_xy(0, ROW_LINE_2, "Vp", 1, 1);
	range_scale(screen1_data.Vpeak, val_buf, range_buf);
	lcd_puts_xy(VALUE_COLUMN, ROW_LINE_2, val_buf, 1, 1);
	lcd_puts_xy(VALUE_RANGE_COLUMN, ROW_LINE_2, range_buf, 1, 1);
	lcd_puts_xy(UNITS_COLUMN, ROW_LINE_2, "V", 1, 1);

	lcd_puts_xy(0, ROW_LINE_3_2, "Irms", 1, 1);
	range_scale(screen1_data.Irms, val_buf, range_buf);
	lcd_puts_xy(VALUE_COLUMN, ROW_LINE_3_2, val_buf, 1, 1);
	lcd_puts_xy(VALUE_RANGE_COLUMN, ROW_LINE_3_2, range_buf, 1, 1);
	lcd_puts_xy(UNITS_COLUMN, ROW_LINE_3_2, "A", 1, 1);

	lcd_puts_xy(0, ROW_LINE_4, "Ip", 1, 1);
	range_scale(screen1_data.Ipeak, val_buf, range_buf);
	lcd_puts_xy(VALUE_COLUMN, ROW_LINE_4, val_buf, 1, 1);
	lcd_puts_xy(VALUE_RANGE_COLUMN, ROW_LINE_4, range_buf, 1, 1);
	lcd_puts_xy(UNITS_COLUMN, ROW_LINE_4, "A", 1, 1);

	LCD5110_Refresh();
}

void lcd_screen_2_refresh() {
	char val_buf[10];
	char range_buf[2];
	LCD5110_Clear();

	lcd_puts_xy(0, ROW_LINE_1, "P", 1, 1);
	range_scale(screen2_data.ActivePower, val_buf, range_buf);
	lcd_puts_xy(VALUE_COLUMN, ROW_LINE_1, val_buf, 1, 1);
	lcd_puts_xy(VALUE_RANGE_COLUMN, ROW_LINE_1, range_buf, 1, 1);
	lcd_puts_xy(UNITS_COLUMN, ROW_LINE_1, "W", 1, 1);

	lcd_puts_xy(0, ROW_LINE_2, "Q", 1, 1);
	range_scale(screen2_data.ReactivePower, val_buf, range_buf);
	lcd_puts_xy(VALUE_COLUMN, ROW_LINE_2, val_buf, 1, 1);
	lcd_puts_xy(VALUE_RANGE_COLUMN, ROW_LINE_2, range_buf, 1, 1);
	lcd_puts_xy(UNITS_COLUMN, ROW_LINE_2, "VAR", 1, 1);

	lcd_puts_xy(0, ROW_LINE_3_1, "S", 1, 1);
	range_scale(screen2_data.ApparantPower, val_buf, range_buf);
	lcd_puts_xy(VALUE_COLUMN, ROW_LINE_3_1, val_buf, 1, 1);
	lcd_puts_xy(VALUE_RANGE_COLUMN, ROW_LINE_3_1, range_buf, 1, 1);
	lcd_puts_xy(UNITS_COLUMN, ROW_LINE_3_1, "VA", 1, 1);

	lcd_puts_xy(0, ROW_LINE_4, "PF", 1, 1);
	sprintf(val_buf, "%.2f", screen2_data.PowerFactor);
	lcd_puts_xy(VALUE_COLUMN, ROW_LINE_4, val_buf, 1, 1);

	LCD5110_Refresh();
}

void lcd_screen_3_refresh() {
	LCD5110_Clear();

	char val_buf[10];
	char range_buf[2];

	lcd_puts_xy(0, ROW_LINE_1, "P.t", 1, 1);
	range_scale(screen3_data.ActiveEnergy, val_buf, range_buf);
	lcd_puts_xy(VALUE_COLUMN, ROW_LINE_1, val_buf, 1, 1);
	lcd_puts_xy(VALUE_RANGE_COLUMN, ROW_LINE_1, range_buf, 1, 1);
	lcd_puts_xy(UNITS_COLUMN, ROW_LINE_1, "Wh", 1, 1);

	lcd_puts_xy(0, ROW_LINE_2, "S.t", 1, 1);
	range_scale(screen3_data.ApparantEnergy, val_buf, range_buf);
	lcd_puts_xy(VALUE_COLUMN, ROW_LINE_2, val_buf, 1, 1);
	lcd_puts_xy(VALUE_RANGE_COLUMN, ROW_LINE_2, range_buf, 1, 1);
	lcd_puts_xy(UNITS_COLUMN, ROW_LINE_2, "VAh", 1, 1);

	if(screen3_mode == S3_RESET_CONFIRM) {
		lcd_puts_xy(12, ROW_LINE_3_1, "Clear Data?", 1, 1);
		lcd_puts_xy(12, ROW_LINE_3_1 + 8, "[#]Y / [*]N", 1, 1);
	}

	char days_buf[5];
	sprintf(days_buf, "%04lu", Screen3_Timer.days);
	lcd_puts_xy(0, ROW_LINE_4, days_buf, 1, 1);
	lcd_puts_xy(26, ROW_LINE_4 + 2, "DAY(s)", 1, 0);

	char runtime_buf[9];
	sprintf(runtime_buf, "%02u:%02u:%02u", Screen3_Timer.hours, Screen3_Timer.minutes, Screen3_Timer.seconds);
	lcd_puts_xy(56, ROW_LINE_4 + 2, runtime_buf, 1, 0);

	LCD5110_Refresh();
}

void lcd_screen_4_refresh() {
	char date_buf[11];
	char time_buf[9];
	char val_buf[10];
	char range_buf[2];
	char interval_buf[16];

	LCD5110_Clear();

	if(screen4_mode == S4_NORMAL_DISPLAY) {

		lcd_puts_xy(0, 0, "PKV", 1, 1);
		range_scale(screen4_data.User_PKI, val_buf, range_buf);
		lcd_puts_xy(VALUE_COLUMN, 0, val_buf, 1, 1);
		lcd_puts_xy(VALUE_RANGE_COLUMN, 0, range_buf, 1, 1);
		lcd_puts_xy(UNITS_COLUMN, 0, "V", 1, 1);

		lcd_puts_xy(0, 10, "PKI", 1, 1);
		range_scale(screen4_data.User_PKI, val_buf, range_buf);
		lcd_puts_xy(VALUE_COLUMN, 10, val_buf, 1, 1);
		lcd_puts_xy(VALUE_RANGE_COLUMN, 10, range_buf, 1, 1);
		lcd_puts_xy(UNITS_COLUMN, 10, "A", 1, 1);

		lcd_puts_xy(0, 20, "SAG", 1, 1);
		range_scale(screen4_data.User_SAG, val_buf, range_buf);
		lcd_puts_xy(VALUE_COLUMN, 20, val_buf, 1, 1);
		lcd_puts_xy(VALUE_RANGE_COLUMN, 20, range_buf, 1, 1);
		lcd_puts_xy(UNITS_COLUMN, 20, "V", 1, 1);

		sprintf(interval_buf, "Interval %dmins", Sample_Interval);
		lcd_puts_xy(0, 30, interval_buf, 1, 1);

		sprintf(date_buf, "%02u/%02u/20%02u", Screen4_RTC.date.date, Screen4_RTC.date.month, Screen4_RTC.date.year);
		lcd_puts_xy(0, ROW_LINE_4 + 2, date_buf, 1, 0);
		sprintf(time_buf, "%02u:%02u:%02u", Screen4_RTC.time.hours, Screen4_RTC.time.minutes, Screen4_RTC.time.seconds);
		lcd_puts_xy(56, ROW_LINE_4 + 2, time_buf, 1, 0);

	} else {
		lcd_puts_xy(0, ROW_LINE_1, "Configuration", 1, 1);

		if(screen4_mode == S4_CONFIG_DISPLAY) {

			lcd_puts_xy(0, ROW_LINE_1, "Configuration", 1, 1);

			if(Screen4_Config_Target <= Screen4_Config_sag) {

				lcd_puts_xy(0, ROW_LINE_2, "PKV", 1, 1);
				range_scale(screen4_data_config.User_PKI, val_buf, range_buf);
				lcd_puts_xy(VALUE_COLUMN, ROW_LINE_2, val_buf, 1, 1);
				lcd_puts_xy(VALUE_RANGE_COLUMN, ROW_LINE_2, range_buf, 1, 1);
				lcd_puts_xy(UNITS_COLUMN, ROW_LINE_2, "V", 1, 1);

				lcd_puts_xy(0, ROW_LINE_3_1, "PKI", 1, 1);
				range_scale(screen4_data_config.User_PKI, val_buf, range_buf);
				lcd_puts_xy(VALUE_COLUMN, ROW_LINE_3_1, val_buf, 1, 1);
				lcd_puts_xy(VALUE_RANGE_COLUMN, ROW_LINE_3_1, range_buf, 1, 1);
				lcd_puts_xy(UNITS_COLUMN, ROW_LINE_3_1, "A", 1, 1);

				lcd_puts_xy(0, ROW_LINE_4, "SAG", 1, 1);
				range_scale(screen4_data_config.User_SAG, val_buf, range_buf);
				lcd_puts_xy(VALUE_COLUMN, ROW_LINE_4, val_buf, 1, 1);
				lcd_puts_xy(VALUE_RANGE_COLUMN, ROW_LINE_4, range_buf, 1, 1);
				lcd_puts_xy(UNITS_COLUMN, ROW_LINE_4, "V", 1, 1);

			} else {

				sprintf(interval_buf, "Interval %dmins", Sample_Interval_Config);
				lcd_puts_xy(0, ROW_LINE_2, interval_buf, 1, 1);

				sprintf(date_buf, "%02u/%02u/20%02u", Screen4_RTC_Config.date.date, Screen4_RTC_Config.date.month, Screen4_RTC_Config.date.year);
				lcd_puts_xy(0, ROW_LINE_3_2, date_buf, 1, 1);
				sprintf(time_buf, "%02u:%02u:%02u", Screen4_RTC_Config.time.hours, Screen4_RTC_Config.time.minutes, Screen4_RTC_Config.time.seconds);
				lcd_puts_xy(0, ROW_LINE_4, time_buf, 1, 1);

			}

		} else {

			lcd_puts_xy(24, ROW_LINE_2, "Commit?", 1, 1);
			lcd_puts_xy(12, ROW_LINE_3_1, "[#]Y / [*]N", 1, 1);

		}
	}

	LCD5110_Refresh();
}

void lcd_screen_refresh(LCD_Screen_t screen) {
	switch (screen) {
		case LCD_Screen_1:
			lcd_screen_1_refresh();
			break;
		case LCD_Screen_2:
			lcd_screen_2_refresh();
			break;
		case LCD_Screen_3:
			lcd_screen_3_refresh();
			break;
		case LCD_Screen_4:
			lcd_screen_4_refresh();
			break;
		default:
			break;
	}
}

void lcd_screen_1_data_update(LCD_Data_Screen1_t data) {
	screen1_data = data;
}

void lcd_screen_2_data_update(LCD_Data_Screen2_t data) {
	screen2_data = data;
}

void lcd_screen_3_data_update(LCD_Data_Screen3_t data) {
	screen3_data = data;
}

void lcd_screen_4_data_update(LCD_Data_Screen4_t data) {
	screen4_data = data;
}

void lcd_screen_1_clear() {
	memset(&screen1_data, 0, sizeof(screen1_data));
}

void lcd_screen_2_clear() {
	memset(&screen2_data, 0, sizeof(screen2_data));
}

void lcd_screen_3_clear() {
	screen3_mode = S3_NORMAL_DISPLAY;
	memset(&screen3_data, 0, sizeof(screen3_data));
	memset(&Screen3_Timer, 0, sizeof(Screen3_Timer));
}

void lcd_screen_4_clear() {
	Sample_Interval = Sample_Interval_60;
	screen4_mode = S4_NORMAL_DISPLAY;
	memset(&screen4_data, 0, sizeof(screen4_data));
	memset(&Screen4_RTC, 0, sizeof(Screen4_RTC));
}

void lcd_screen_3_timer_count_up() {
	Screen3_Timer.seconds++;
	if(Screen3_Timer.seconds > 59) {
		Screen3_Timer.seconds -= 60;
		Screen3_Timer.minutes++;
		if(Screen3_Timer.minutes > 59) {
			Screen3_Timer.minutes -= 60;
			Screen3_Timer.hours++;
			if(Screen3_Timer.hours > 23) {
				Screen3_Timer.hours -= 24;
				Screen3_Timer.days++;
			}
		}
	}
}

void lcd_screen_3_switch_mode() {
	screen3_mode ^= 1;
}

LCD_Screen3_Mode lcd_screen_3_mode() {
	return screen3_mode;
}

void lcd_screen_4_rtc_update(LCD_Screen4_RTC_t datetime) {
	Screen4_RTC = datetime;
	if(screen4_mode == S4_NORMAL_DISPLAY) Screen4_RTC_Config = datetime;
}

void lcd_screen_4_switch_mode() {
	if(++screen4_mode == S4_No_of_Mode)
		screen4_mode = 0;
}

LCD_Screen4_Mode lcd_screen_4_mode() {
	return screen4_mode;
}

void lcd_screen_4_next_config_target() {
	if(++Screen4_Config_Target == Screen4_No_of_Config_items)
		Screen4_Config_Target = 0;
}

void lcd_screen_4_next_interval_set() {
	switch(Sample_Interval) {
		case Sample_Interval_5:
			Sample_Interval = Sample_Interval_15;
			break;
		case Sample_Interval_15:
			Sample_Interval = Sample_Interval_30;
			break;
		case Sample_Interval_30:
			Sample_Interval = Sample_Interval_60;
			break;
		case Sample_Interval_60:
		default:
			Sample_Interval = Sample_Interval_5;
			break;
	}
}

LCD_Screen4_RTC_t lcd_screen_4_commit_config(Config_Commit_t commit) {
	if(commit == CONFIG_COMMIT) {
		screen4_data = screen4_data_config;
		Sample_Interval = Sample_Interval_Config;
		Screen4_RTC = Screen4_RTC_Config;
	}

	lcd_screen_4_switch_mode();
	return Screen4_RTC;
}

bool lcd_is_busy() {
	return screen3_mode || screen4_mode;
}
