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

static struct {
	uint32_t days;
	uint8_t hours;
	uint8_t minutes;
	uint8_t seconds;
} Screen3_Timer;

void lcd_puts_xy(unsigned char x, unsigned char y, char* c, int font) {
	LCD5110_GotoXY(x, y);

	if (font)
		LCD5110_Puts(c, LCD5110_Pixel_Set, LCD5110_FontSize_5x7);
	else
		LCD5110_Puts(c, LCD5110_Pixel_Set, LCD5110_FontSize_3x5);
}

void range_scale(float value, char* value_buf, char* range) {
	if(value < 1000) {
		sprintf(range, "%s", "m");
	} else if (value < 1000*1000) {
		sprintf(range, "%s", " ");
		value /= 1000;
	} else if (value < 1000*1000*1000) {
		sprintf(range, "%s", "k");
		value /= 1000*1000;
	}

	if(value < 10) {
		sprintf(value_buf, "%1.3f", value);
	} else if (value < 100) {
		sprintf(value_buf, "%2.2f", value);
	} else if (value < 1000) {
		sprintf(value_buf, "%3.1f", value);
	}
}

void lcd_screen_1_update(LCD_Data_Screen1_t data) {
	char val_buf[10];
	char range_buf[2];
	LCD5110_Clear();

	lcd_puts_xy(0, ROW_LINE_1, "Vrms", 1);
	range_scale(data.Vrms, val_buf, range_buf);
	lcd_puts_xy(VALUE_COLUMN, ROW_LINE_1, val_buf, 1);
	lcd_puts_xy(VALUE_RANGE_COLUMN, ROW_LINE_1, range_buf, 1);
	lcd_puts_xy(UNITS_COLUMN, ROW_LINE_1, "V", 1);

	lcd_puts_xy(0, ROW_LINE_2, "Vp", 1);
	range_scale(data.Vpeak, val_buf, range_buf);
	lcd_puts_xy(VALUE_COLUMN, ROW_LINE_2, val_buf, 1);
	lcd_puts_xy(VALUE_RANGE_COLUMN, ROW_LINE_2, range_buf, 1);
	lcd_puts_xy(UNITS_COLUMN, ROW_LINE_2, "V", 1);

	lcd_puts_xy(0, ROW_LINE_3_2, "Irms", 1);
	range_scale(data.Irms, val_buf, range_buf);
	lcd_puts_xy(VALUE_COLUMN, ROW_LINE_3_2, val_buf, 1);
	lcd_puts_xy(VALUE_RANGE_COLUMN, ROW_LINE_3_2, range_buf, 1);
	lcd_puts_xy(UNITS_COLUMN, ROW_LINE_3_2, "A", 1);

	lcd_puts_xy(0, ROW_LINE_4, "Ip", 1);
	range_scale(data.Ipeak, val_buf, range_buf);
	lcd_puts_xy(VALUE_COLUMN, ROW_LINE_4, val_buf, 1);
	lcd_puts_xy(VALUE_RANGE_COLUMN, ROW_LINE_4, range_buf, 1);
	lcd_puts_xy(UNITS_COLUMN, ROW_LINE_4, "A", 1);

	LCD5110_Refresh();
}

void lcd_screen_2_update(LCD_Data_Screen2_t data) {
	char val_buf[10];
	char range_buf[2];
	LCD5110_Clear();

	lcd_puts_xy(0, ROW_LINE_1, "P", 1);
	range_scale(data.ActivePower, val_buf, range_buf);
	lcd_puts_xy(VALUE_COLUMN, ROW_LINE_1, val_buf, 1);
	lcd_puts_xy(VALUE_RANGE_COLUMN, ROW_LINE_1, range_buf, 1);
	lcd_puts_xy(UNITS_COLUMN, ROW_LINE_1, "W", 1);

	lcd_puts_xy(0, ROW_LINE_2, "Q", 1);
	range_scale(data.ReactivePower, val_buf, range_buf);
	lcd_puts_xy(VALUE_COLUMN, ROW_LINE_2, val_buf, 1);
	lcd_puts_xy(VALUE_RANGE_COLUMN, ROW_LINE_2, range_buf, 1);
	lcd_puts_xy(UNITS_COLUMN, ROW_LINE_2, "VAR", 1);

	lcd_puts_xy(0, ROW_LINE_3_1, "S", 1);
	range_scale(data.ApparantPower, val_buf, range_buf);
	lcd_puts_xy(VALUE_COLUMN, ROW_LINE_3_1, val_buf, 1);
	lcd_puts_xy(VALUE_RANGE_COLUMN, ROW_LINE_3_1, range_buf, 1);
	lcd_puts_xy(UNITS_COLUMN, ROW_LINE_3_1, "VA", 1);

	lcd_puts_xy(0, ROW_LINE_4, "PF", 1);
	sprintf(val_buf, "%.2f", data.PowerFactor);
	lcd_puts_xy(VALUE_COLUMN, ROW_LINE_4, val_buf, 1);

	LCD5110_Refresh();
}

void lcd_screen_3_update(LCD_Data_Screen3_t data) {
	char val_buf[10];
	char range_buf[2];
	LCD5110_Clear();

	lcd_puts_xy(0, ROW_LINE_1, "P.t", 1);
	range_scale(data.ActiveEnergy, val_buf, range_buf);
	lcd_puts_xy(VALUE_COLUMN, ROW_LINE_1, val_buf, 1);
	lcd_puts_xy(VALUE_RANGE_COLUMN, ROW_LINE_1, range_buf, 1);
	lcd_puts_xy(UNITS_COLUMN, ROW_LINE_1, "Wh", 1);

	lcd_puts_xy(0, ROW_LINE_2, "S.t", 1);
	range_scale(data.ApparantEnergy, val_buf, range_buf);
	lcd_puts_xy(VALUE_COLUMN, ROW_LINE_2, val_buf, 1);
	lcd_puts_xy(VALUE_RANGE_COLUMN, ROW_LINE_2, range_buf, 1);
	lcd_puts_xy(UNITS_COLUMN, ROW_LINE_2, "VAh", 1);

	char days_buf[12];
	sprintf(days_buf, "%04lu day(s)", Screen3_Timer.days);
	lcd_puts_xy(0, ROW_LINE_3_2, days_buf, 1);

	char runtime_buf[9];
	sprintf(runtime_buf, "%02u:%02u:%02u", Screen3_Timer.hours, Screen3_Timer.minutes, Screen3_Timer.seconds);
	lcd_puts_xy(0, ROW_LINE_4, runtime_buf, 1);

	LCD5110_Refresh();
}

void lcd_screen_4_update(LCD_Data_Screen4_t data) {
	LCD5110_Clear();

	LCD5110_Refresh();
}

void lcd_screen_3_reset() {
	memset(&Screen3_Timer, 0, sizeof(Screen3_Timer));
}

void lcd_screen_4_reset() {

}

void lcd_screen_3_timer_update(uint8_t second_update) {
	if(second_update) {
		Screen3_Timer.seconds += second_update;
		if(Screen3_Timer.seconds > 59) {
			Screen3_Timer.seconds -= 60;
			Screen3_Timer.minutes++;
			if(Screen3_Timer.minutes > 59) {
				Screen3_Timer.minutes -= 60;
				Screen3_Timer.hours++;
				if(Screen3_Timer.hours > 23) Screen3_Timer.days++;
			}
		}
	}
}

