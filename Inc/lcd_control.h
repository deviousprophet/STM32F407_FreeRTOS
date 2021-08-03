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
#include "keypad.h"
#include "ade7753.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

typedef struct {
	uint32_t days;
	uint8_t hours;
	uint8_t minutes;
	uint8_t seconds;
} Screen_Timer_t;

typedef enum {
	S3_NORMAL_DISPLAY,
	S3_RESET_CONFIRM_DISPLAY
} LCD_Screen3_Mode;

typedef enum {
	S4_NORMAL_DISPLAY,
	S4_CONFIG_DISPLAY,
	S4_CONFIG_PARAMS_DISPLAY,
	S4_CONFIG_DATETIME_DISPLAY,
	S4_COMMIT_DISPLAY
} LCD_Screen4_Mode;

typedef enum {
	CONFIG_CANCEL,
	CONFIG_COMMIT
} Config_Commit_t;

typedef enum {
	CONFIG_DESELECT,
	CONFIG_SELECT
} Config_Select_t;

typedef enum {
	Sample_Interval_5 = 5,
	Sample_Interval_15 = 15,
	Sample_Interval_30 = 30,
	Sample_Interval_60 = 60
} Sample_Interval_t;

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
//	float ApparantEnergy;
	Screen_Timer_t EnergyTimer;
} LCD_Data_Screen3_t;

typedef struct {
	float User_PKV;
	float User_PKI;
	float User_SAG;
	Sample_Interval_t User_Interval;
} LCD_Data_Screen4_t;

typedef enum {
	Config_pkv,
	Config_pkv_selected,
	Config_pki,
	Config_pki_selected,
	Config_sag,
	Config_sag_selected,
	Config_interval,
	Config_interval_selected,
	Config_date,
	Config_date_selected,
	Config_time,
	Config_time_selected,
	Max_Config_Items
} Config_Items_t;

typedef enum {
	Config_Params,
	Config_DateTime
} Config_Option_t;

void lcd_screen_refresh(LCD_Screen_t screen);

void lcd_screen_1_data_update(LCD_Data_Screen1_t data);
void lcd_screen_2_data_update(LCD_Data_Screen2_t data);
void lcd_screen_3_data_update(LCD_Data_Screen3_t data);
void lcd_screen_4_data_update(LCD_Data_Screen4_t data);

void lcd_screen_1_clear();
void lcd_screen_2_clear();
void lcd_screen_3_clear();
void lcd_screen_4_clear();

void lcd_screen_3_switch_mode(LCD_Screen3_Mode mode);
LCD_Screen3_Mode lcd_screen_3_mode();

void lcd_screen_4_rtc_update(DS1307_DateTime_t datetime);
void lcd_screen_4_switch_mode(LCD_Screen4_Mode mode);
LCD_Screen4_Mode lcd_screen_4_mode();
Config_Option_t lcd_screen_4_config_option();
Config_Items_t lcd_screen_4_config_item();
void lcd_screen_4_next_config_option();
void lcd_screen_4_next_config_item();
void lcd_screen_4_config_select(Config_Select_t select);
void lcd_screen_4_next_interval_set();
void lcd_screen_4_next_rangei_set();

DS1307_DateTime_t lcd_screen_4_commit_rtc();
LCD_Data_Screen4_t lcd_screen_4_commit_parameters();

void lcd_enter_datetime_value(KEYPAD_Button_t key);
void lcd_enter_params_value(KEYPAD_Button_t key);

bool lcd_is_busy();

#endif /* LCD_CONTROL_H_ */
