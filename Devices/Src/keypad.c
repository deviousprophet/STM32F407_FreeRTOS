/*
 * keypad4x4.c
 *
 *  Created on: Jun 12, 2021
 *      Author: deviousprophet
 */


#include "keypad.h"

/* Pins configuration, rows are outputs */
#define KEYPAD_ROW_1_LOW			GPIO_WriteToOutputPin(KEYPAD_ROW_1_PORT, KEYPAD_ROW_1_PIN, GPIO_PIN_RESET)
#define KEYPAD_ROW_2_LOW			GPIO_WriteToOutputPin(KEYPAD_ROW_2_PORT, KEYPAD_ROW_2_PIN, GPIO_PIN_RESET)
#define KEYPAD_ROW_3_LOW			GPIO_WriteToOutputPin(KEYPAD_ROW_3_PORT, KEYPAD_ROW_3_PIN, GPIO_PIN_RESET)
#define KEYPAD_ROW_4_LOW			GPIO_WriteToOutputPin(KEYPAD_ROW_4_PORT, KEYPAD_ROW_4_PIN, GPIO_PIN_RESET)

#define KEYPAD_ROW_1_HIGH			GPIO_WriteToOutputPin(KEYPAD_ROW_1_PORT, KEYPAD_ROW_1_PIN, GPIO_PIN_SET)
#define KEYPAD_ROW_2_HIGH			GPIO_WriteToOutputPin(KEYPAD_ROW_2_PORT, KEYPAD_ROW_2_PIN, GPIO_PIN_SET)
#define KEYPAD_ROW_3_HIGH			GPIO_WriteToOutputPin(KEYPAD_ROW_3_PORT, KEYPAD_ROW_3_PIN, GPIO_PIN_SET)
#define KEYPAD_ROW_4_HIGH			GPIO_WriteToOutputPin(KEYPAD_ROW_4_PORT, KEYPAD_ROW_4_PIN, GPIO_PIN_SET)

/* Read input pins */
#define KEYPAD_COLUMN_1_CHECK		(!GPIO_ReadFromInputPin(KEYPAD_COLUMN_1_PORT, KEYPAD_COLUMN_1_PIN))
#define KEYPAD_COLUMN_2_CHECK		(!GPIO_ReadFromInputPin(KEYPAD_COLUMN_2_PORT, KEYPAD_COLUMN_2_PIN))
#define KEYPAD_COLUMN_3_CHECK		(!GPIO_ReadFromInputPin(KEYPAD_COLUMN_3_PORT, KEYPAD_COLUMN_3_PIN))
#define KEYPAD_COLUMN_4_CHECK		(!GPIO_ReadFromInputPin(KEYPAD_COLUMN_4_PORT, KEYPAD_COLUMN_4_PIN))

uint8_t KEYPAD_INT_Buttons[4][4] = {
	{0x01, 0x02, 0x03, 0x0C},
	{0x04, 0x05, 0x06, 0x0D},
	{0x07, 0x08, 0x09, 0x0E},
	{0x0A, 0x00, 0x0B, 0x0F},
};

/* Private functions */
void KEYPAD_INT_SetRow(uint8_t row);
uint8_t KEYPAD_INT_CheckColumn(uint8_t row);
uint8_t KEYPAD_INT_Read(void);

/* Private variables */
KEYPAD_Type_t KEYPAD_INT_KeypadType;
static KEYPAD_Button_t KeypadStatus = KEYPAD_NOPRESSED;

void KEYPAD_Init(KEYPAD_Type_t type) {
	KEYPAD_INT_KeypadType = type;
	GPIO_Handle_t GpioR_Out, GpioC_In;

	/* Rows are output */
	/* Row 1 */
	GpioR_Out.pGPIOx = KEYPAD_ROW_1_PORT;
	GpioR_Out.GPIO_PinConfig.GPIO_PinNumber = KEYPAD_ROW_1_PIN;
	GpioR_Out.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	GpioR_Out.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_MEDIUM;
	GpioR_Out.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
	GpioR_Out.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;
	GPIO_Init(&GpioR_Out);

	/* Row 2 */
	GpioR_Out.pGPIOx = KEYPAD_ROW_2_PORT;
	GpioR_Out.GPIO_PinConfig.GPIO_PinNumber = KEYPAD_ROW_2_PIN;
	GPIO_Init(&GpioR_Out);

	/* Row 3 */
	GpioR_Out.pGPIOx = KEYPAD_ROW_3_PORT;
	GpioR_Out.GPIO_PinConfig.GPIO_PinNumber = KEYPAD_ROW_3_PIN;
	GPIO_Init(&GpioR_Out);

	/* Row 4 */
	if (type == KEYPAD_Type_Large) {
		GpioR_Out.pGPIOx = KEYPAD_ROW_4_PORT;
		GpioR_Out.GPIO_PinConfig.GPIO_PinNumber = KEYPAD_ROW_4_PIN;
		GPIO_Init(&GpioR_Out);
	}


	/* Columns are inputs */
	/* Column 1 */
	GpioC_In.pGPIOx = KEYPAD_COLUMN_1_PORT;
	GpioC_In.GPIO_PinConfig.GPIO_PinNumber = KEYPAD_COLUMN_1_PIN;
	GpioC_In.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IN;
	GpioC_In.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_MEDIUM;
	GpioC_In.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
	GpioC_In.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PIN_PU;
	GPIO_Init(&GpioC_In);

	/* Column 2 */
	GpioC_In.pGPIOx = KEYPAD_COLUMN_2_PORT;
	GpioC_In.GPIO_PinConfig.GPIO_PinNumber = KEYPAD_COLUMN_2_PIN;
	GPIO_Init(&GpioC_In);

	/* Column 3 */
	GpioC_In.pGPIOx = KEYPAD_COLUMN_3_PORT;
	GpioC_In.GPIO_PinConfig.GPIO_PinNumber = KEYPAD_COLUMN_3_PIN;
	GPIO_Init(&GpioC_In);

	/* Column 4 */
	GpioC_In.pGPIOx = KEYPAD_COLUMN_4_PORT;
	GpioC_In.GPIO_PinConfig.GPIO_PinNumber = KEYPAD_COLUMN_4_PIN;
	GPIO_Init(&GpioC_In);

	/* All rows high */
	KEYPAD_INT_SetRow(0);
}

KEYPAD_Button_t KEYPAD_Read(void) {
	return KeypadStatus;
}

/* Private */
void KEYPAD_INT_SetRow(uint8_t row) {
	/* Set rows high */
	KEYPAD_ROW_1_HIGH;
	KEYPAD_ROW_2_HIGH;
	KEYPAD_ROW_3_HIGH;
	if (KEYPAD_INT_KeypadType == KEYPAD_Type_Large)
		KEYPAD_ROW_4_HIGH;

	/* Set row low */
	if (row == 1)
		KEYPAD_ROW_1_LOW;
	if (row == 2)
		KEYPAD_ROW_2_LOW;
	if (row == 3)
		KEYPAD_ROW_3_LOW;
	if (row == 4)
		KEYPAD_ROW_4_LOW;
}

uint8_t KEYPAD_INT_CheckColumn(uint8_t row) {
	/* Read columns */

	/* Scan column 1 */
	if (KEYPAD_COLUMN_1_CHECK) {
		return KEYPAD_INT_Buttons[row - 1][0];
	}
	/* Scan column 2 */
	if (KEYPAD_COLUMN_2_CHECK) {
		return KEYPAD_INT_Buttons[row - 1][1];
	}
	/* Scan column 3 */
	if (KEYPAD_COLUMN_3_CHECK) {
		return KEYPAD_INT_Buttons[row - 1][2];
	}
	/* Scan column 4 */
	if (KEYPAD_INT_KeypadType == KEYPAD_Type_Large && KEYPAD_COLUMN_4_CHECK) {
		return KEYPAD_INT_Buttons[row - 1][3];
	}

	/* Not pressed */
	return KEYPAD_NOPRESSED;
}

uint8_t KEYPAD_INT_Read(void) {
	uint8_t check;
	/* Set row 1 to LOW */
	KEYPAD_INT_SetRow(1);
	/* Check columns */
	check = KEYPAD_INT_CheckColumn(1);
	if (check != KEYPAD_NOPRESSED) {
		return check;
	}

	/* Set row 2 to LOW */
	KEYPAD_INT_SetRow(2);
	/* Check columns */
	check = KEYPAD_INT_CheckColumn(2);
	if (check != KEYPAD_NOPRESSED) {
		return check;
	}

	/* Set row 3 to LOW */
	KEYPAD_INT_SetRow(3);
	/* Check columns */
	check = KEYPAD_INT_CheckColumn(3);
	if (check != KEYPAD_NOPRESSED) {
		return check;
	}

	if (KEYPAD_INT_KeypadType == KEYPAD_Type_Large) {
		/* Set row 4 to LOW */
		KEYPAD_INT_SetRow(4);
		/* Check columns */
		check = KEYPAD_INT_CheckColumn(4);
		if (check != KEYPAD_NOPRESSED) {
			return check;
		}
	}

	/* Not pressed */
	return KEYPAD_NOPRESSED;
}

void KEYPAD_Update(void) {
	static uint16_t millis = 0;
	if (++millis >= KEYPAD_READ_INTERVAL) {
		millis = 0;
		KeypadStatus = (KEYPAD_Button_t) KEYPAD_INT_Read();
	}
}
