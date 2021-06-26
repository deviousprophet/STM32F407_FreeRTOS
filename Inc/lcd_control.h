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

typedef enum {
	LCD_Display_Mode,
	LCD_Config_Mode
} LCD_ScreenMode_t;

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

void lcd_screen_1_update(LCD_Data_Screen1_t data);
void lcd_screen_2_update(LCD_Data_Screen2_t data);
void lcd_screen_3_update(LCD_Data_Screen3_t data);
void lcd_screen_4_update(LCD_Data_Screen4_t data);

#endif /* LCD_CONTROL_H_ */
