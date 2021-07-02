/*
 * lcd_control.h
 *
 *  Created on: Jun 13, 2021
 *      Author: deviousprophet
 */

#ifndef LCD_CONTROL_H_
#define LCD_CONTROL_H_

#include "lcd5110.h"
#include "ds1307.h"
#include <stdio.h>
#include <string.h>

typedef enum {
	LCD_Screen_1,
	LCD_Screen_2,
	LCD_Screen_3,
	LCD_Screen_4
} LCD_Screen_t;

typedef struct {
	float Vrms;
	float Vpeak;
	float Irms;
	float Ipeak;
} LCD_Data_Screen1_t;

typedef struct {
	float ActivePower;
	float ReactivePower;
	float ApparantPower;
	float PowerFactor;
} LCD_Data_Screen2_t;

typedef struct {
	float ActiveEnergy;
	float ApparantEnergy;
} LCD_Data_Screen3_t;

typedef struct {
	float UserActiveEnergy;
	float UserReactiveEnergy;
	float UserApparantEnergy;
} LCD_Data_Screen4_t;

typedef enum {
	NORMAL_DISPLAY,
	CONFIG_DISPLAY
} LCD_Screen4_Mode;

typedef struct {
	RTC_date_t date;
	RTC_time_t time;
} LCD_Screen4_RTC_t;

void lcd_screen_1_refresh();
void lcd_screen_2_refresh();
void lcd_screen_3_refresh();
void lcd_screen_4_refresh();

void lcd_screen_1_data_update(LCD_Data_Screen1_t data);
void lcd_screen_2_data_update(LCD_Data_Screen2_t data);
void lcd_screen_3_data_update(LCD_Data_Screen3_t data);
void lcd_screen_4_data_update(LCD_Data_Screen4_t data);

void lcd_screen_1_reset();
void lcd_screen_2_reset();
void lcd_screen_3_reset();
void lcd_screen_4_reset();

void lcd_screen_3_timer_count_up();

void lcd_screen_4_rtc_update(LCD_Screen4_RTC_t datetime);
LCD_Screen4_Mode lcd_screen_4_mode();
void lcd_screen_4_switch_mode();

#endif /* LCD_CONTROL_H_ */
