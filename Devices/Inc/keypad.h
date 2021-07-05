/*
 * keypad.h
 *
 *  Created on: Jun 12, 2021
 *      Author: deviousprophet
 */

#ifndef KEYPAD_H_
#define KEYPAD_H_

#include "stm32f407xx.h"
#include "stm32f407xx_gpio.h"

/* Rows */
/* Row 1 default */
#ifndef KEYPAD_ROW_1_PIN
#define KEYPAD_ROW_1_PORT				GPIOC
#define KEYPAD_ROW_1_PIN				GPIO_PIN_NO_11
#endif
/* Row 2 default */
#ifndef KEYPAD_ROW_2_PIN
#define KEYPAD_ROW_2_PORT				GPIOD
#define KEYPAD_ROW_2_PIN				GPIO_PIN_NO_0
#endif
/* Row 3 default */
#ifndef KEYPAD_ROW_3_PIN
#define KEYPAD_ROW_3_PORT				GPIOD
#define KEYPAD_ROW_3_PIN				GPIO_PIN_NO_2
#endif
/* Row 4 default */
#ifndef KEYPAD_ROW_4_PIN
#define KEYPAD_ROW_4_PORT				GPIOD
#define KEYPAD_ROW_4_PIN				GPIO_PIN_NO_4
#endif

/* Columns */
/* Column 1 default */
#ifndef KEYPAD_COLUMN_1_PIN
#define KEYPAD_COLUMN_1_PORT			GPIOD
#define KEYPAD_COLUMN_1_PIN				GPIO_PIN_NO_6
#endif
/* Column 2 default */
#ifndef KEYPAD_COLUMN_2_PIN
#define KEYPAD_COLUMN_2_PORT			GPIOB
#define KEYPAD_COLUMN_2_PIN				GPIO_PIN_NO_3
#endif
/* Column 3 default */
#ifndef KEYPAD_COLUMN_3_PIN
#define KEYPAD_COLUMN_3_PORT			GPIOB
#define KEYPAD_COLUMN_3_PIN				GPIO_PIN_NO_5
#endif
/* Column 4 default */
#ifndef KEYPAD_COLUMN_4_PIN
#define KEYPAD_COLUMN_4_PORT			GPIOB
#define KEYPAD_COLUMN_4_PIN				GPIO_PIN_NO_7
#endif

typedef enum {
	KEYPAD_NOPRESSED 		= (uint8_t) 0xFF, 		/*!< No button pressed */
	KEYPAD_Button_0 		= (uint8_t) 0x00,       /*!< Button 0 code */
	KEYPAD_Button_1 		= (uint8_t) 0x01,       /*!< Button 1 code */
	KEYPAD_Button_2 		= (uint8_t) 0x02,       /*!< Button 2 code */
	KEYPAD_Button_3 		= (uint8_t) 0x03,       /*!< Button 3 code */
	KEYPAD_Button_4 		= (uint8_t) 0x04,       /*!< Button 4 code */
	KEYPAD_Button_5 		= (uint8_t) 0x05,       /*!< Button 5 code */
	KEYPAD_Button_6 		= (uint8_t) 0x06,       /*!< Button 6 code */
	KEYPAD_Button_7 		= (uint8_t) 0x07,       /*!< Button 7 code */
	KEYPAD_Button_8 		= (uint8_t) 0x08,       /*!< Button 8 code */
	KEYPAD_Button_9 		= (uint8_t) 0x09,       /*!< Button 9 code */
	KEYPAD_Button_STAR 		= (uint8_t) 0x0A,       /*!< Button START code */
	KEYPAD_Button_HASH 		= (uint8_t) 0x0B,       /*!< Button HASH code */
	KEYPAD_Button_A 		= (uint8_t) 0x0C,	    /*!< Button A code. Only on large size */
	KEYPAD_Button_B 		= (uint8_t) 0x0D,	    /*!< Button B code. Only on large size */
	KEYPAD_Button_C 		= (uint8_t) 0x0E,	    /*!< Button C code. Only on large size */
	KEYPAD_Button_D 		= (uint8_t) 0x0F,	    /*!< Button D code. Only on large size */

//	Button HOLD code
	KEYPAD_Button_HOLD_0 	= (uint8_t) 0x80,
	KEYPAD_Button_HOLD_1 	= (uint8_t) 0x81,
	KEYPAD_Button_HOLD_2 	= (uint8_t) 0x82,
	KEYPAD_Button_HOLD_3 	= (uint8_t) 0x83,
	KEYPAD_Button_HOLD_4 	= (uint8_t) 0x84,
	KEYPAD_Button_HOLD_5 	= (uint8_t) 0x85,
	KEYPAD_Button_HOLD_6 	= (uint8_t) 0x86,
	KEYPAD_Button_HOLD_7 	= (uint8_t) 0x87,
	KEYPAD_Button_HOLD_8 	= (uint8_t) 0x88,
	KEYPAD_Button_HOLD_9 	= (uint8_t) 0x89,
	KEYPAD_Button_HOLD_STAR = (uint8_t) 0x8A,
	KEYPAD_Button_HOLD_HASH = (uint8_t) 0x8B,
	KEYPAD_Button_HOLD_A 	= (uint8_t) 0x8C,
	KEYPAD_Button_HOLD_B 	= (uint8_t) 0x8D,
	KEYPAD_Button_HOLD_C 	= (uint8_t) 0x8E,
	KEYPAD_Button_HOLD_D 	= (uint8_t) 0x8F,

} KEYPAD_Button_t;

typedef enum {
	KEYPAD_Type_Large, 		/*!< Keypad 4x4 size */
	KEYPAD_Type_Small		/*!< Keypad 3x4 size */
} KEYPAD_Type_t;

void KEYPAD_Init(KEYPAD_Type_t type);

KEYPAD_Button_t KEYPAD_Read();

KEYPAD_Button_t KEYPAD_Hold_Button(KEYPAD_Button_t button);

void KEYPAD_Update();

#endif /* KEYPAD_H_ */
