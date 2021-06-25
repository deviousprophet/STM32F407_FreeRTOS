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

/**
 * @defgroup KEYPAD_Macros
 * @brief    Library defines
 * @{
 */

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

/* Number of milliseconds between 2 reads */
#ifndef KEYPAD_READ_INTERVAL
#define KEYPAD_READ_INTERVAL        50
#endif

/**
 * @brief  Keypad button enumeration
 */
typedef enum {
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
	KEYPAD_NOPRESSED 		= (uint8_t) 0xFF 		/*!< No button pressed */
} KEYPAD_Button_t;

/**
 * @brief  Keypad size enumeration
 */
typedef enum {
	KEYPAD_Type_Large = 0, /*!< Keypad 4x4 size */
	KEYPAD_Type_Small         /*!< Keypad 3x4 size */
} KEYPAD_Type_t;

/**
 * @defgroup KEYPAD_Functions
 * @brief    Library Functions
 * @{
 */

/**
 * @brief  Initializes keypad functionality
 * @param  type: Keypad type you will use. This parameter can be a value of @ref KEYPAD_Type_t enumeration
 * @retval None
 */
void KEYPAD_Init(KEYPAD_Type_t type);

/**
 * @brief  Reads keypad data
 * @param  None
 * @retval Button status. This parameter will be a value of @ref KEYPAD_Button_t enumeration
 */
KEYPAD_Button_t KEYPAD_Read(void);

/**
 * @brief  Updates keypad
 * @note   This function must be called from interrupt routine every 1ms
 * @param  None
 * @retval None
 */
void KEYPAD_Update(void);

#endif /* KEYPAD_H_ */
