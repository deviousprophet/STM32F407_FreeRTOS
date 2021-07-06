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
#include <stdbool.h>
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
	float User_PKV;
	float User_PKI;
	float User_SAG;
} LCD_Data_Screen4_t;

typedef enum {
	S3_NORMAL_DISPLAY,
	S3_RESET_CONFIRM,
} LCD_Screen3_Mode;

typedef enum {
	S4_NORMAL_DISPLAY,
	S4_CONFIG_DISPLAY,
	S4_COMMIT_DISPLAY,
	S4_No_of_Mode
} LCD_Screen4_Mode;

typedef struct {
	RTC_date_t date;
	RTC_time_t time;
} LCD_Screen4_RTC_t;

typedef enum {
	CONFIG_CANCEL,
	CONFIG_COMMIT
} Config_Commit_t;

typedef enum {
	CONFIG_DESELECT,
	CONFIG_SELECT
} Config_Select_t;

void lcd_screen_refresh(LCD_Screen_t screen);

void lcd_screen_1_data_update(LCD_Data_Screen1_t data);
void lcd_screen_2_data_update(LCD_Data_Screen2_t data);
void lcd_screen_3_data_update(LCD_Data_Screen3_t data);
void lcd_screen_4_data_update(LCD_Data_Screen4_t data);

void lcd_screen_1_clear();
void lcd_screen_2_clear();
void lcd_screen_3_clear();
void lcd_screen_4_clear();

void lcd_screen_3_timer_count_up();
void lcd_screen_3_switch_mode();
LCD_Screen3_Mode lcd_screen_3_mode();

void lcd_screen_4_rtc_update(LCD_Screen4_RTC_t datetime);
void lcd_screen_4_switch_mode();
LCD_Screen4_Mode lcd_screen_4_mode();
void lcd_screen_4_next_config_target();
void lcd_screen_4_config_target(Config_Select_t select);
void lcd_screen_4_next_interval_set();
LCD_Screen4_RTC_t lcd_screen_4_commit_config(Config_Commit_t commit);

bool lcd_is_busy();

#endif /* LCD_CONTROL_H_ */
