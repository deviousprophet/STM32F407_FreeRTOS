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
#define INTVL_COLUMN			40

typedef struct {
	uint32_t days;
	uint8_t hours;
	uint8_t minutes;
	uint8_t seconds;
} Screen_Timer_t;

LCD_Screen3_Mode screen3_mode;
Screen_Timer_t screen3_timer;

LCD_Screen4_Mode screen4_mode;
Config_Option_t screen4_config_option;
Config_Items_t screen4_config_item;

LCD_Data_Screen1_t screen1_data;
LCD_Data_Screen2_t screen2_data;
LCD_Data_Screen3_t screen3_data;
LCD_Data_Screen4_t screen4_data, screen4_data_config;

Device_RTC_t rtc_display, rtc_config;

void lcd_puts_xy(unsigned char x, unsigned char y, char* c, LCD5110_Pixel_t color, LCD5110_FontSize_t font) {
	LCD5110_GotoXY(x, y);
	LCD5110_Puts(c, color, font);
}

void range_scale(float value, char* value_buf, char* range) {
	if(value < 0) value *= -1;
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

	if(screen3_mode == S3_RESET_CONFIRM_DISPLAY) {
		lcd_puts_xy(12, ROW_LINE_3_1, "Clear Data?", 1, 1);
		lcd_puts_xy(12, ROW_LINE_3_1 + 8, "[#]Y / [*]N", 1, 1);
	}

	char days_buf[5];
	sprintf(days_buf, "%04lu", screen3_timer.days);
	lcd_puts_xy(0, ROW_LINE_4, days_buf, 1, 1);
	lcd_puts_xy(26, ROW_LINE_4 + 2, "DAY(s)", 1, 0);

	char runtime_buf[9];
	sprintf(runtime_buf, "%02u:%02u:%02u", screen3_timer.hours, screen3_timer.minutes, screen3_timer.seconds);
	lcd_puts_xy(56, ROW_LINE_4 + 2, runtime_buf, 1, 0);

	LCD5110_Refresh();
}

void lcd_screen_4_refresh() {
	char date_buf[11];
	char time_buf[9];
	char val_buf[10];
	char range_buf[2];
	char interval_buf[7];

	LCD5110_Clear();

	if(screen4_mode == S4_NORMAL_DISPLAY) {

		lcd_puts_xy(0, 0, "PKV", 1, 1);
		range_scale(screen4_data.User_PKV, val_buf, range_buf);
		lcd_puts_xy(VALUE_COLUMN, 0, val_buf, 1, 1);
//		lcd_puts_xy(VALUE_RANGE_COLUMN, 0, range_buf, 1, 1);
		lcd_puts_xy(UNITS_COLUMN, 0, "V", 1, 1);

		lcd_puts_xy(0, 10, "PKI", 1, 1);
		range_scale(screen4_data.User_PKI, val_buf, range_buf);
		lcd_puts_xy(VALUE_COLUMN, 10, val_buf, 1, 1);
//		lcd_puts_xy(VALUE_RANGE_COLUMN, 10, range_buf, 1, 1);
		lcd_puts_xy(UNITS_COLUMN, 10, "A", 1, 1);

		lcd_puts_xy(0, 20, "SAG", 1, 1);
		range_scale(screen4_data.User_SAG, val_buf, range_buf);
		lcd_puts_xy(VALUE_COLUMN, 20, val_buf, 1, 1);
//		lcd_puts_xy(VALUE_RANGE_COLUMN, 20, range_buf, 1, 1);
		lcd_puts_xy(UNITS_COLUMN, 20, "V", 1, 1);

		lcd_puts_xy(0, 30, "Intv", 1, 1);
		sprintf(interval_buf, "%02dmins", screen4_data.User_Interval);
		lcd_puts_xy(INTVL_COLUMN, 30, interval_buf, 1, 1);

		sprintf(date_buf, "%02u/%02u/20%02u",
				rtc_display.date.date,
				rtc_display.date.month,
				rtc_display.date.year);
		lcd_puts_xy(0, ROW_LINE_4 + 2, date_buf, 1, 0);
		sprintf(time_buf, "%02u:%02u:%02u",
				rtc_display.time.hours,
				rtc_display.time.minutes,
				rtc_display.time.seconds);
		lcd_puts_xy(56, ROW_LINE_4 + 2, time_buf, 1, 0);

	} else {

		lcd_puts_xy(5, 0, "Configuration", 1, 1);

		if(screen4_mode == S4_CONFIG_DISPLAY) {
			lcd_puts_xy(10, 10, "Parameters", 1, 1);
			lcd_puts_xy(10, 20, "Date & Time", 1, 1);
			if(screen4_config_option == Config_Params)
				lcd_puts_xy(0, 10, ">", 1, 1);
			else lcd_puts_xy(0, 20, ">", 1, 1);

			sprintf(date_buf, "%02u/%02u/20%02u",
					rtc_display.date.date,
					rtc_display.date.month,
					rtc_display.date.year);
			lcd_puts_xy(0, ROW_LINE_4 + 2, date_buf, 1, 0);
			sprintf(time_buf, "%02u:%02u:%02u",
					rtc_display.time.hours,
					rtc_display.time.minutes,
					rtc_display.time.seconds);
			lcd_puts_xy(56, ROW_LINE_4 + 2, time_buf, 1, 0);

		} else if (screen4_mode == S4_CONFIG_PARAMS_DISPLAY) {
			lcd_puts_xy(0, 10, "PKV", 1, 1);
			lcd_puts_xy(0, 20, "PKI", 1, 1);
			lcd_puts_xy(0, 30, "SAG", 1, 1);
			lcd_puts_xy(0, 40, "Intv", 1, 1);

			switch (screen4_config_item) {
				case Config_pkv:
					lcd_puts_xy(20, 10, "<", 1, 1);
					break;
				case Config_pkv_selected:
					lcd_puts_xy(20, 10, ">", 1, 1);
					break;
				case Config_pki:
					lcd_puts_xy(20, 20, "<", 1, 1);
					break;
				case Config_pki_selected:
					lcd_puts_xy(20, 20, ">", 1, 1);
					break;
				case Config_sag:
					lcd_puts_xy(20, 30, "<", 1, 1);
					break;
				case Config_sag_selected:
					lcd_puts_xy(20, 30, ">", 1, 1);
					break;
				case Config_interval:
					lcd_puts_xy(26, 40, "<", 1, 1);
					break;
				case Config_interval_selected:
					lcd_puts_xy(26, 40, ">", 1, 1);
					break;
				default:
					break;
			}

			range_scale(screen4_data_config.User_PKV, val_buf, range_buf);
			lcd_puts_xy(VALUE_COLUMN, 10, val_buf, 1, 1);
//			lcd_puts_xy(VALUE_RANGE_COLUMN, 10, range_buf, 1, 1);
			lcd_puts_xy(UNITS_COLUMN, 10, "V", 1, 1);

			range_scale(screen4_data_config.User_PKI, val_buf, range_buf);
			lcd_puts_xy(VALUE_COLUMN, 20, val_buf, 1, 1);
//			lcd_puts_xy(VALUE_RANGE_COLUMN, 20, range_buf, 1, 1);
			lcd_puts_xy(UNITS_COLUMN, 20, "A", 1, 1);

			range_scale(screen4_data_config.User_SAG, val_buf, range_buf);
			lcd_puts_xy(VALUE_COLUMN, 30, val_buf, 1, 1);
//			lcd_puts_xy(VALUE_RANGE_COLUMN, 30, range_buf, 1, 1);
			lcd_puts_xy(UNITS_COLUMN, 30, "V", 1, 1);

			sprintf(interval_buf, "%02dmins", screen4_data_config.User_Interval);
			lcd_puts_xy(INTVL_COLUMN, 40, interval_buf, 1, 1);


		} else if (screen4_mode == S4_CONFIG_DATETIME_DISPLAY) {
			lcd_puts_xy(0, 10, "Date", 1, 1);
			lcd_puts_xy(0, 30, "Time", 1, 1);

			switch (screen4_config_item) {
				case Config_date:
					lcd_puts_xy(26, 10, "<", 1, 1);
					break;
				case Config_date_selected:
					lcd_puts_xy(0, 20, ">", 1, 1);
					break;
				case Config_time:
					lcd_puts_xy(26, 30, "<", 1, 1);
					break;
				case Config_time_selected:
					lcd_puts_xy(0, 40, ">", 1, 1);
					break;
				default:
					break;
			}

			sprintf(date_buf, "%02d/%02d/20%02d",
					rtc_config.date.date,
					rtc_config.date.month,
					rtc_config.date.year);
			lcd_puts_xy(20, 20, date_buf, 1, 1);

			sprintf(time_buf, "%02d:%02d:%02d",
					rtc_config.time.hours,
					rtc_config.time.minutes,
					rtc_config.time.seconds);
			lcd_puts_xy(20, 40, time_buf, 1, 1);

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
	memset(&screen3_data, 0, sizeof(screen3_data));
	memset(&screen3_timer, 0, sizeof(screen3_timer));
	screen3_mode = S3_NORMAL_DISPLAY;
}

void lcd_screen_4_clear() {
	memset(&screen4_data, 0, sizeof(screen4_data));
	screen4_data.User_Interval = Sample_Interval_60;
	screen4_mode = S4_NORMAL_DISPLAY;
	screen4_config_option = Config_Params;
	screen4_config_item = Config_pkv;
}

void lcd_screen_3_timer_count_up() {
	screen3_timer.seconds++;
	if(screen3_timer.seconds > 59) {
		screen3_timer.seconds -= 60;
		screen3_timer.minutes++;
		if(screen3_timer.minutes > 59) {
			screen3_timer.minutes -= 60;
			screen3_timer.hours++;
			if(screen3_timer.hours > 23) {
				screen3_timer.hours -= 24;
				screen3_timer.days++;
			}
		}
	}
}

void lcd_screen_3_switch_mode(LCD_Screen3_Mode mode) {
	screen3_mode = mode;
}

LCD_Screen3_Mode lcd_screen_3_mode() {
	return screen3_mode;
}

void lcd_screen_4_rtc_update(Device_RTC_t datetime) {
	rtc_display = datetime;
}

void lcd_screen_4_switch_mode(LCD_Screen4_Mode mode) {
	if(mode == S4_CONFIG_PARAMS_DISPLAY) {
		if(screen4_mode == S4_CONFIG_DISPLAY) {
			screen4_config_item = Config_pkv;
			screen4_data_config = screen4_data;
		}
	}

	if(mode == S4_CONFIG_DATETIME_DISPLAY) {
		if(screen4_mode == S4_CONFIG_DISPLAY) {
			screen4_config_item = Config_date;
			rtc_config = rtc_display;
		}
	}

	if(mode == S4_CONFIG_DISPLAY) {
		if(screen4_mode == S4_NORMAL_DISPLAY)
			screen4_config_option = Config_Params;
	}
	screen4_mode = mode;
}

LCD_Screen4_Mode lcd_screen_4_mode() {
	return screen4_mode;
}

Config_Option_t lcd_screen_4_config_option() {
	return screen4_config_option;
}

Config_Items_t lcd_screen_4_config_item() {
	return screen4_config_item;
}

void lcd_screen_4_next_config_option() {
	screen4_config_option ^= 1;
}

void lcd_screen_4_next_config_item() {
	if(!(screen4_config_item % 2)) {
		screen4_config_item += 2;
		if(screen4_mode == S4_CONFIG_PARAMS_DISPLAY) {
			if(screen4_config_item > Config_interval)
				screen4_config_item = Config_pkv;
		}
		if(screen4_mode == S4_CONFIG_DATETIME_DISPLAY) {
			if(screen4_config_item == Max_Config_Items)
				screen4_config_item = Config_date;
		}
	}
}

void lcd_screen_4_config_select(Config_Select_t select) {
	if(select == CONFIG_SELECT) {
		if(!(screen4_config_item % 2)) {
			switch (++screen4_config_item) {
				case Config_pkv_selected:
				case Config_pki_selected:
				case Config_sag_selected:
					lcd_enter_params_value(0xff);
					break;
				case Config_date_selected:
				case Config_time_selected:
					lcd_enter_datetime_value(0xff);
					break;
				default:
					break;
			}
		}
	} else if(screen4_config_item % 2) screen4_config_item--;
}

void lcd_screen_4_next_interval_set() {
	switch(screen4_data_config.User_Interval) {
		case Sample_Interval_5:
			screen4_data_config.User_Interval = Sample_Interval_15;
			break;
		case Sample_Interval_15:
			screen4_data_config.User_Interval = Sample_Interval_30;
			break;
		case Sample_Interval_30:
			screen4_data_config.User_Interval = Sample_Interval_60;
			break;
		case Sample_Interval_60:
			screen4_data_config.User_Interval = Sample_Interval_5;
			break;
		default:
			break;
	}
}

Device_RTC_t lcd_screen_4_commit_rtc() {
	return rtc_config;
}

LCD_Data_Screen4_t lcd_screen_4_commit_parameters() {
	return screen4_data = screen4_data_config;
}

void lcd_enter_datetime_value(KEYPAD_Button_t key) {
	static uint32_t base_value = 0;
	static uint32_t range = 100000;

	switch (key) {
		case KEYPAD_Button_0:
		case KEYPAD_Button_1:
		case KEYPAD_Button_2:
		case KEYPAD_Button_3:
		case KEYPAD_Button_4:
		case KEYPAD_Button_5:
		case KEYPAD_Button_6:
		case KEYPAD_Button_7:
		case KEYPAD_Button_8:
		case KEYPAD_Button_9:
			if(true) {
				base_value += ((uint8_t) key) * range;
				range /= 10;
			}
			break;
		default:
			base_value = 0;
			range = 100000;
			break;
	}

	if(screen4_config_item == Config_date_selected) {
		rtc_config.date.day = MONDAY;
		rtc_config.date.date = base_value / 10000;
		rtc_config.date.month = (base_value % 10000) / 100;
		rtc_config.date.year = base_value % 100;
	}
	if(screen4_config_item == Config_time_selected) {
		rtc_config.time.hours = base_value / 10000;
		rtc_config.time.minutes = (base_value % 10000) / 100;
		rtc_config.time.seconds = base_value % 100;
		rtc_config.time.time_format = TIME_FORMAT_24HRS;
	}
}

void lcd_enter_params_value(KEYPAD_Button_t key) {
	static uint16_t base_value = 0;
	static uint16_t afterpoint = 0;

	switch (key) {
		case KEYPAD_Button_0:
		case KEYPAD_Button_1:
		case KEYPAD_Button_2:
		case KEYPAD_Button_3:
		case KEYPAD_Button_4:
		case KEYPAD_Button_5:
		case KEYPAD_Button_6:
		case KEYPAD_Button_7:
		case KEYPAD_Button_8:
		case KEYPAD_Button_9:
			if(base_value < 1000 && afterpoint < 1000) {
				base_value = base_value * 10 + ((uint8_t) key);
				if(afterpoint) afterpoint *= 10;
				else if(base_value >= 1000) afterpoint = 10;
			}
			break;
		case KEYPAD_Button_D:
			if(!afterpoint) afterpoint = 1;
			break;
		default:
			base_value = 0;
			afterpoint = 0;
			break;
	}

	float key_input = (float) base_value;
	if(afterpoint) key_input /= afterpoint;

	switch (screen4_config_item) {
		case Config_pkv_selected:
			screen4_data_config.User_PKV = key_input;
			break;
		case Config_pki_selected:
			screen4_data_config.User_PKI = key_input;
			break;
		case Config_sag_selected:
			screen4_data_config.User_SAG = key_input;
			break;
		default:
			break;
	}
}

bool lcd_is_busy() {
	return screen3_mode || screen4_mode;
}
