/*
 * lcd5110.c
 *
 *  Created on: Jun 12, 2021
 *      Author: deviousprophet
 */


#include "lcd5110.h"

unsigned char LCD5110_Buffer[LCD5110_BUFFER_SIZE];
unsigned char LCD5110_UpdateXmin = 0, LCD5110_UpdateXmax = 0, LCD5110_UpdateYmin = 0, LCD5110_UpdateYmax = 0;
unsigned char LCD5110_x;
unsigned char LCD5110_y;

//Fonts 5x7
const uint8_t LCD5110_Font5x7 [97][LCD5110_CHAR5x7_WIDTH] = {
	{ 0x00, 0x00, 0x00, 0x00, 0x00 },   // sp
	{ 0x00, 0x00, 0x2f, 0x00, 0x00 },   // !
	{ 0x00, 0x07, 0x00, 0x07, 0x00 },   // "
	{ 0x14, 0x7f, 0x14, 0x7f, 0x14 },   // #
	{ 0x24, 0x2a, 0x7f, 0x2a, 0x12 },   // $
	{ 0x32, 0x34, 0x08, 0x16, 0x26 },   // %
	{ 0x36, 0x49, 0x55, 0x22, 0x50 },   // &
	{ 0x00, 0x05, 0x03, 0x00, 0x00 },   // '
	{ 0x00, 0x1c, 0x22, 0x41, 0x00 },   // (
	{ 0x00, 0x41, 0x22, 0x1c, 0x00 },   // )
	{ 0x14, 0x08, 0x3E, 0x08, 0x14 },   // *
	{ 0x08, 0x08, 0x3E, 0x08, 0x08 },   // +
	{ 0x00, 0x00, 0x50, 0x30, 0x00 },   // ,
	{ 0x10, 0x10, 0x10, 0x10, 0x10 },   // -
	{ 0x00, 0x60, 0x60, 0x00, 0x00 },   // .
	{ 0x20, 0x10, 0x08, 0x04, 0x02 },   // /
	{ 0x3E, 0x51, 0x49, 0x45, 0x3E },   // 0
	{ 0x00, 0x42, 0x7F, 0x40, 0x00 },   // 1
	{ 0x42, 0x61, 0x51, 0x49, 0x46 },   // 2
	{ 0x21, 0x41, 0x45, 0x4B, 0x31 },   // 3
	{ 0x18, 0x14, 0x12, 0x7F, 0x10 },   // 4
	{ 0x27, 0x45, 0x45, 0x45, 0x39 },   // 5
	{ 0x3C, 0x4A, 0x49, 0x49, 0x30 },   // 6
	{ 0x01, 0x71, 0x09, 0x05, 0x03 },   // 7
	{ 0x36, 0x49, 0x49, 0x49, 0x36 },   // 8
	{ 0x06, 0x49, 0x49, 0x29, 0x1E },   // 9
	{ 0x00, 0x36, 0x36, 0x00, 0x00 },   // :
	{ 0x00, 0x56, 0x36, 0x00, 0x00 },   // ;
	{ 0x08, 0x14, 0x22, 0x41, 0x00 },   // <
	{ 0x14, 0x14, 0x14, 0x14, 0x14 },   // =
	{ 0x00, 0x41, 0x22, 0x14, 0x08 },   // >
	{ 0x02, 0x01, 0x51, 0x09, 0x06 },   // ?
	{ 0x32, 0x49, 0x59, 0x51, 0x3E },   // @
	{ 0x7E, 0x11, 0x11, 0x11, 0x7E },   // A
	{ 0x7F, 0x49, 0x49, 0x49, 0x36 },   // B
	{ 0x3E, 0x41, 0x41, 0x41, 0x22 },   // C
	{ 0x7F, 0x41, 0x41, 0x22, 0x1C },   // D
	{ 0x7F, 0x49, 0x49, 0x49, 0x41 },   // E
	{ 0x7F, 0x09, 0x09, 0x09, 0x01 },   // F
	{ 0x3E, 0x41, 0x49, 0x49, 0x7A },   // G
	{ 0x7F, 0x08, 0x08, 0x08, 0x7F },   // H
	{ 0x00, 0x41, 0x7F, 0x41, 0x00 },   // I
	{ 0x20, 0x40, 0x41, 0x3F, 0x01 },   // J
	{ 0x7F, 0x08, 0x14, 0x22, 0x41 },   // K
	{ 0x7F, 0x40, 0x40, 0x40, 0x40 },   // L
	{ 0x7F, 0x02, 0x0C, 0x02, 0x7F },   // M
	{ 0x7F, 0x04, 0x08, 0x10, 0x7F },   // N
	{ 0x3E, 0x41, 0x41, 0x41, 0x3E },   // O
	{ 0x7F, 0x09, 0x09, 0x09, 0x06 },   // P
	{ 0x3E, 0x41, 0x51, 0x21, 0x5E },   // Q
	{ 0x7F, 0x09, 0x19, 0x29, 0x46 },   // R
	{ 0x46, 0x49, 0x49, 0x49, 0x31 },   // S
	{ 0x01, 0x01, 0x7F, 0x01, 0x01 },   // T
	{ 0x3F, 0x40, 0x40, 0x40, 0x3F },   // U
	{ 0x1F, 0x20, 0x40, 0x20, 0x1F },   // V
	{ 0x3F, 0x40, 0x38, 0x40, 0x3F },   // W
	{ 0x63, 0x14, 0x08, 0x14, 0x63 },   // X
	{ 0x07, 0x08, 0x70, 0x08, 0x07 },   // Y
	{ 0x61, 0x51, 0x49, 0x45, 0x43 },   // Z
	{ 0x00, 0x7F, 0x41, 0x41, 0x00 },   // [
	{ 0x55, 0x2A, 0x55, 0x2A, 0x55 },   // 55
	{ 0x00, 0x41, 0x41, 0x7F, 0x00 },   // ]
	{ 0x04, 0x02, 0x01, 0x02, 0x04 },   // ^
	{ 0x40, 0x40, 0x40, 0x40, 0x40 },   // _
	{ 0x00, 0x01, 0x02, 0x04, 0x00 },   // '
	{ 0x20, 0x54, 0x54, 0x54, 0x78 },   // a
	{ 0x7F, 0x48, 0x44, 0x44, 0x38 },   // b
	{ 0x38, 0x44, 0x44, 0x44, 0x20 },   // c
	{ 0x38, 0x44, 0x44, 0x48, 0x7F },   // d
	{ 0x38, 0x54, 0x54, 0x54, 0x18 },   // e
	{ 0x08, 0x7E, 0x09, 0x01, 0x02 },   // f
	{ 0x0C, 0x52, 0x52, 0x52, 0x3E },   // g
	{ 0x7F, 0x08, 0x04, 0x04, 0x78 },   // h
	{ 0x00, 0x44, 0x7D, 0x40, 0x00 },   // i
	{ 0x20, 0x40, 0x44, 0x3D, 0x00 },   // j
	{ 0x7F, 0x10, 0x28, 0x44, 0x00 },   // k
	{ 0x00, 0x41, 0x7F, 0x40, 0x00 },   // l
	{ 0x7C, 0x04, 0x18, 0x04, 0x78 },   // m
	{ 0x7C, 0x08, 0x04, 0x04, 0x78 },   // n
	{ 0x38, 0x44, 0x44, 0x44, 0x38 },   // o
	{ 0x7C, 0x14, 0x14, 0x14, 0x08 },   // p
	{ 0x08, 0x14, 0x14, 0x18, 0x7C },   // q
	{ 0x7C, 0x08, 0x04, 0x04, 0x08 },   // r
	{ 0x48, 0x54, 0x54, 0x54, 0x20 },   // s
	{ 0x04, 0x3F, 0x44, 0x40, 0x20 },   // t
	{ 0x3C, 0x40, 0x40, 0x20, 0x7C },   // u
	{ 0x1C, 0x20, 0x40, 0x20, 0x1C },   // v
	{ 0x3C, 0x40, 0x30, 0x40, 0x3C },   // w
	{ 0x44, 0x28, 0x10, 0x28, 0x44 },   // x
	{ 0x0C, 0x50, 0x50, 0x50, 0x3C },   // y
	{ 0x44, 0x64, 0x54, 0x4C, 0x44 },   // z
	{ 0x00, 0x7F, 0x3E, 0x1C, 0x08 },   // > Filled		123
	{ 0x08, 0x1C, 0x3E, 0x7F, 0x00 }, 	 // < Filled	124
	{ 0x08, 0x7C, 0x7E, 0x7C, 0x08 },   // Arrow up		125
	{ 0x10, 0x3E, 0x7E, 0x3E, 0x10 },   // Arrow down	126
	{ 0x3E, 0x3E, 0x3E, 0x3E, 0x3E },   // Stop			127
	{ 0x00, 0x7F, 0x3E, 0x1C, 0x08 }    // Play			128
};

const uint8_t LCD5110_Font3x5[106][3] = {
	{ 0x00, 0x00, 0x00 },   // sp - 32
	{ 0x00, 0x17, 0x00 },   // ! - 33
	{ 0x03, 0x00, 0x03 },   // " - 34
	{ 0x1F, 0x0A, 0x1F },   // # - 35
	{ 0x0A, 0x1F, 0x05 },   // $
	{ 0x09, 0x04, 0x12 },   // %
	{ 0x0F, 0x17, 0x1C },   // &
	{ 0x00, 0x03, 0x00 },   // '
	{ 0x00, 0x0E, 0x11 },   // ( - 40
	{ 0x11, 0x0E, 0x00 },   // )
	{ 0x05, 0x02, 0x05 },   // *
	{ 0x04, 0x0E, 0x04 },   // +
	{ 0x10, 0x08, 0x00 },   // ,
	{ 0x04, 0x04, 0x04 },   // - - 45
	{ 0x00, 0x10, 0x00 },   // .
	{ 0x08, 0x04, 0x02 },   // /
	{ 0x1F, 0x11, 0x1F },   // 0
	{ 0x12, 0x1F, 0x10 },   // 1
	{ 0x1D, 0x15, 0x17 },   // 2 - 50
	{ 0x11, 0x15, 0x1F },   // 3
	{ 0x07, 0x04, 0x1F },   // 4
	{ 0x17, 0x15, 0x1D },   // 5
	{ 0x1F, 0x15, 0x1D },   // 6
	{ 0x01, 0x01, 0x1F },   // 7 - 55
	{ 0x1F, 0x15, 0x1F },   // 8
	{ 0x17, 0x15, 0x1F },   // 9 - 57
	{ 0x00, 0x0A, 0x00 },   // :
	{ 0x10, 0x0A, 0x00 },   // ;
	{ 0x04, 0x0A, 0x11 },   // < - 60
	{ 0x0A, 0x0A, 0x0A },   // =
	{ 0x11, 0x0A, 0x04 },   // >
	{ 0x01, 0x15, 0x03 },   // ?
	{ 0x0E, 0x15, 0x16 },   // @
	{ 0x1E, 0x05, 0x1E },   // A - 65
	{ 0x1F, 0x15, 0x0A },   // B
	{ 0x0E, 0x11, 0x11 },   // C
	{ 0x1F, 0x11, 0x0E },   // D
	{ 0x1F, 0x15, 0x15 },   // E
	{ 0x1F, 0x05, 0x05 },   // F - 70
	{ 0x0E, 0x15, 0x1D },   // G
	{ 0x1F, 0x04, 0x1F },   // H
	{ 0x11, 0x1F, 0x11 },   // I
	{ 0x08, 0x10, 0x0F },   // J
	{ 0x1F, 0x04, 0x1B },   // K - 75
	{ 0x1F, 0x10, 0x10 },   // L
	{ 0x1F, 0x06, 0x1F },   // M
	{ 0x1F, 0x0E, 0x1F },   // N
	{ 0x0E, 0x11, 0x0E },   // O
	{ 0x1F, 0x05, 0x02 },   // P - 80
	{ 0x0E, 0x11, 0x1E },   // Q
	{ 0x1F, 0x0D, 0x16 },   // R
	{ 0x12, 0x15, 0x09 },   // S
	{ 0x01, 0x1F, 0x01 },   // T
	{ 0x0F, 0x10, 0x0F },   // U - 85
	{ 0x07, 0x18, 0x07 },   // V
	{ 0x1F, 0x0C, 0x1F },   // W
	{ 0x1B, 0x04, 0x1B },   // X
	{ 0x03, 0x1C, 0x03 },   // Y
	{ 0x19, 0x15, 0x13 },   // Z - 90
	{ 0x1F, 0x11, 0x00 },   // [
	{ 0x02, 0x04, 0x08 },   // 55 - backspace - 92
	{ 0x00, 0x11, 0x1F },   // ]
	{ 0x02, 0x01, 0x02 },   // ^
	{ 0x10, 0x10, 0x10 },   // _ - 95
	{ 0x01, 0x02, 0x00 },   // `
	{ 0x1A, 0x16, 0x1C },   // a
	{ 0x1F, 0x12, 0x0C },   // b
	{ 0x0C, 0x12, 0x12 },   // c
	{ 0x0C, 0x12, 0x1F },   // d - 100
	{ 0x0C, 0x1A, 0x16 },   // e
	{ 0x04, 0x1E, 0x05 },   // f
	{ 0x06, 0x15, 0x0F },   // g
	{ 0x1F, 0x02, 0x1C },   // h
	{ 0x00, 0x1D, 0x00 },   // i - 105
	{ 0x10, 0x10, 0x0D },   // j
	{ 0x1F, 0x0C, 0x12 },   // k
	{ 0x11, 0x1F, 0x10 },   // l
	{ 0x1E, 0x0E, 0x1E },   // m
	{ 0x1E, 0x02, 0x1C },   // n - 110
	{ 0x0C, 0x12, 0x0C },   // o
	{ 0x1E, 0x0A, 0x04 },   // p
	{ 0x04, 0x0A, 0x1E },   // q
	{ 0x1C, 0x02, 0x02 },   // r
	{ 0x14, 0x1E, 0x0A },   // s - 115
	{ 0x02, 0x1F, 0x12 },   // t
	{ 0x0E, 0x10, 0x1E },   // u
	{ 0x0E, 0x10, 0x0E },   // v
	{ 0x1E, 0x1C, 0x1E },   // w
	{ 0x12, 0x0C, 0x12 },   // x - 120
	{ 0x02, 0x14, 0x1E },   // y
	{ 0x1A, 0x1E, 0x16 },   // z
	{ 0x04, 0x1B, 0x11 },   // {
	{ 0x00, 0x1F, 0x00 },   // |
	{ 0x11, 0x1B, 0x04 },   // }
	{ 0x04, 0x06, 0x02 },   // ~
	{ 0x1F, 0x1F, 0x1F },   // delete
};

void LCD5110_send(unsigned char data) {

	SPI_PeripheralControl(LCD5110_SPI, ENABLE); //SS pin pull to LOW
	GPIO_WriteToOutputPin(LCD5110_CE_PORT, LCD5110_CE_PIN, 0);
	SPI_Transfer(LCD5110_SPI, data);
	GPIO_WriteToOutputPin(LCD5110_CE_PORT, LCD5110_CE_PIN, 1);
	SPI_PeripheralControl(LCD5110_SPI, DISABLE); //SS pin pull to HIGH
}

void LCD5110_Pin(LCD5110_Pin_t pin, LCD5110_State_t state) {
	switch (pin) {
		case LCD5110_Pin_DC:
			if (state != LCD5110_State_Low) {
				GPIO_WriteToOutputPin(LCD5110_DC_PORT, LCD5110_DC_PIN, 1);
			} else {
				GPIO_WriteToOutputPin(LCD5110_DC_PORT, LCD5110_DC_PIN, 0);
			}
			break;
		case LCD5110_Pin_RST:
			if (state != LCD5110_State_Low) {
				GPIO_WriteToOutputPin(LCD5110_RST_PORT, LCD5110_RST_PIN,1);
			} else {
				GPIO_WriteToOutputPin(LCD5110_RST_PORT, LCD5110_RST_PIN,0);
			}
			break;
		default: break;
	}
}

void LCD5110_Delay(unsigned long micros) {
	volatile unsigned long i;
	for (i = 0; i < micros; i++);
}

void LCD5110_Init(unsigned char contrast) {
	GPIO_Handle_t SPIPins;

	SPIPins.pGPIOx = GPIOA;
	SPIPins.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	SPIPins.GPIO_PinConfig.GPIO_PinAltFunMode = 5;
	SPIPins.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
	SPIPins.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;
	SPIPins.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;

	//SCLK
	SPIPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_5;
	GPIO_Init(&SPIPins);

	//MOSI
	SPIPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_7;
	GPIO_Init(&SPIPins);

	//CE
	SPIPins.pGPIOx = LCD5110_CE_PORT;
	SPIPins.GPIO_PinConfig.GPIO_PinNumber = LCD5110_CE_PIN;
	SPIPins.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	SPIPins.GPIO_PinConfig.GPIO_PinAltFunMode = 0;
	SPIPins.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
	GPIO_Init(&SPIPins);

	//DC Pin GPIO
	SPIPins.pGPIOx = LCD5110_DC_PORT;
	SPIPins.GPIO_PinConfig.GPIO_PinNumber = LCD5110_DC_PIN;
	GPIO_Init(&SPIPins);

	//RESET Pin GPIO
	SPIPins.pGPIOx = LCD5110_RST_PORT;
	SPIPins.GPIO_PinConfig.GPIO_PinNumber = LCD5110_RST_PIN;
	GPIO_Init(&SPIPins);

	SPI_Handle_t LCD_handle;
	LCD_handle.pSPIx = LCD5110_SPI;
	LCD_handle.SPIConfig.SPI_BusConfig = SPI_BUS_CONFIG_FD;			//full duplex
	LCD_handle.SPIConfig.SPI_DeviceMode = SPI_DEVICE_MODE_MASTER;	//STM as master
	LCD_handle.SPIConfig.SPI_SclkSpeed = SPI_SCLK_SPEED_DIV8;		// clock 2MHz
	LCD_handle.SPIConfig.SPI_DFF = SPI_DFF_8BITS;					// mỗi lần truyền 1 byte
	LCD_handle.SPIConfig.SPI_CPOL = SPI_CPOL_LOW;					//CPOL 0
	LCD_handle.SPIConfig.SPI_CPHA = SPI_CPHA_LOW;					//CPHA 0
	LCD_handle.SPIConfig.SPI_SSM = SPI_SSM_DI; 						//Hardware slave management enabled for NSS pin
	SPI_Init(&LCD_handle);
	SPI_SSOEConfig(LCD5110_SPI, ENABLE);

	//Reset
	LCD5110_Pin(LCD5110_Pin_RST, LCD5110_State_Low);
	LCD5110_Delay(10000);
	LCD5110_Pin(LCD5110_Pin_RST, LCD5110_State_High);

	// Go in extended mode
	LCD5110_Write(LCD5110_COMMAND, LCD5110_FUNCTIONSET | LCD5110_EXTENDEDINSTRUCTION);

	// LCD bias select
	LCD5110_Write(LCD5110_COMMAND, LCD5110_SETBIAS | 0x4);

	// set VOP
	if (contrast > 0x7F) {
		contrast = 0x7F;
	}
	LCD5110_Write(LCD5110_COMMAND, LCD5110_SETVOP | contrast);

	// normal mode
	LCD5110_Write(LCD5110_COMMAND, LCD5110_FUNCTIONSET);

	// Set display to Normal
	LCD5110_Write(LCD5110_COMMAND, LCD5110_DISPLAYCONTROL | LCD5110_DISPLAYNORMAL);

	//Set cursor to home position
	LCD5110_Home();

	//Normal display
	LCD5110_Write(LCD5110_COMMAND, LCD5110_DISPLAYCONTROL | LCD5110_DISPLAYNORMAL);

	//Clear display
	LCD5110_Clear();
}

void LCD5110_Write(LCD5110_WriteType_t cd, unsigned char data) {
	switch (cd) {
		//Send data to lcd's ram
		case LCD5110_DATA:
			//Set DC pin HIGH
			LCD5110_Pin(LCD5110_Pin_DC, LCD5110_State_High);
			break;
		//Send command to lcd
		case LCD5110_COMMAND:
			//Set DC pin LOW
			LCD5110_Pin(LCD5110_Pin_DC, LCD5110_State_Low);
			break;
		default: break;
	}
	//Send data
	LCD5110_send(data);
}

void LCD5110_SetContrast(unsigned char contrast) {
	// Go in extended mode
	LCD5110_Write(LCD5110_COMMAND, LCD5110_FUNCTIONSET | LCD5110_EXTENDEDINSTRUCTION);

	// set VOP
	if (contrast > 0x7F) {
		contrast = 0x7F;
	}
	LCD5110_Write(LCD5110_COMMAND, LCD5110_SETVOP | contrast);

	// normal mode
	LCD5110_Write(LCD5110_COMMAND, LCD5110_FUNCTIONSET);
}

void LCD5110_DrawPixel(unsigned char x, unsigned char y, LCD5110_Pixel_t pixel) {
	if (x >= LCD5110_WIDTH) {
		return;
	}
	if (y >= LCD5110_HEIGHT) {
		return;
	}

	if (pixel != LCD5110_Pixel_Clear) {
		LCD5110_Buffer[x + (y / 8) * LCD5110_WIDTH] |= 1 << (y % 8);
	} else {
		LCD5110_Buffer[x + (y / 8) * LCD5110_WIDTH] &= ~(1 << (y % 8));
	}
	LCD5110_UpdateArea(x, y, x, y);
}

void LCD5110_Invert(LCD5110_Invert_t invert) {
	if (invert != LCD5110_Invert_No) {
		LCD5110_Write(LCD5110_COMMAND, LCD5110_DISPLAYCONTROL | LCD5110_DISPLAYINVERTED);
	} else {
		LCD5110_Write(LCD5110_COMMAND, LCD5110_DISPLAYCONTROL | LCD5110_DISPLAYNORMAL);
	}
}

void LCD5110_Clear(void) {
	unsigned int i;
	LCD5110_Home();
	for (i = 0; i < LCD5110_BUFFER_SIZE; i++) {
		LCD5110_Buffer[i] = 0x00;
		//LCD5110_Write(LCD5110_DATA, 0x00);
	}
	LCD5110_GotoXY(0, 0);
	LCD5110_UpdateArea(0, 0, LCD5110_WIDTH - 1, LCD5110_HEIGHT - 1);
	LCD5110_Refresh();
}

void LCD5110_Home(void) {
	LCD5110_Write(LCD5110_COMMAND, LCD5110_SETXADDR | 0);
	LCD5110_Write(LCD5110_COMMAND, LCD5110_SETYADDR | 0);
}

void LCD5110_Refresh(void) {
	unsigned char i, j;
	for (i = 0; i < 6; i++) {
		//Not in range yet
		if (LCD5110_UpdateYmin > ((i + 1) * 8)) {
			continue;
		}
		//Over range, stop
		if ((i * 8) > LCD5110_UpdateYmax) {
			break;
		}

		LCD5110_Write(LCD5110_COMMAND, LCD5110_SETYADDR | i);
		LCD5110_Write(LCD5110_COMMAND, LCD5110_SETXADDR | LCD5110_UpdateXmin);

		for (j = LCD5110_UpdateXmin; j <= LCD5110_UpdateXmax; j++) {
			LCD5110_Write(LCD5110_DATA, LCD5110_Buffer[(i * LCD5110_WIDTH) + j]);
		}
	}

	LCD5110_UpdateXmin = LCD5110_WIDTH - 1;
	LCD5110_UpdateXmax = 0;
	LCD5110_UpdateYmin = LCD5110_HEIGHT - 1;
	LCD5110_UpdateYmax = 0;
}

void LCD5110_UpdateArea(unsigned char xMin, unsigned char yMin, unsigned char xMax, unsigned char yMax) {
	if (xMin < LCD5110_UpdateXmin) {
		LCD5110_UpdateXmin = xMin;
	}
	if (xMax > LCD5110_UpdateXmax) {
		LCD5110_UpdateXmax = xMax;
	}
	if (yMin < LCD5110_UpdateYmin) {
		LCD5110_UpdateYmin = yMin;
	}
	if (yMax > LCD5110_UpdateYmax) {
		LCD5110_UpdateYmax = yMax;
	}
}

void LCD5110_GotoXY(unsigned char x, unsigned char y) {
	LCD5110_x = x;
	LCD5110_y = y;
}

void LCD5110_Putc(char c, LCD5110_Pixel_t color, LCD5110_FontSize_t size) {
	unsigned char c_height, c_width, i, b, j;
	if (size == LCD5110_FontSize_3x5) {
		c_width = LCD5110_CHAR3x5_WIDTH;
		c_height = LCD5110_CHAR3x5_HEIGHT;
	} else {
		c_width = LCD5110_CHAR5x7_WIDTH;
		c_height = LCD5110_CHAR5x7_HEIGHT;
	}
	if ((LCD5110_x + c_width) > LCD5110_WIDTH) {
		//If at the end of a line of display, go to new line and set x to 0 position
		LCD5110_y += c_height;
		LCD5110_x = 0;
	}
	for (i = 0; i < c_width - 1; i++) {
		if (c < 32) {
			//b = _custom_chars[_font_size][(uint8_t)chr][i];
		} else if (size == LCD5110_FontSize_3x5) {
			b = LCD5110_Font3x5[c - 32][i];
		} else {
			b = LCD5110_Font5x7[c - 32][i];
		}
		if (b == 0x00 && (c != 0 && c != 32)) {
			continue;
		}
		for (j = 0; j < c_height; j++) {
			if (color == LCD5110_Pixel_Set) {
				LCD5110_DrawPixel(LCD5110_x, (LCD5110_y + j), ((b >> j) & 1) ? LCD5110_Pixel_Set : LCD5110_Pixel_Clear);
			} else {
				LCD5110_DrawPixel(LCD5110_x, (LCD5110_y + j), ((b >> j) & 1) ? LCD5110_Pixel_Clear : LCD5110_Pixel_Set);
			}
		}
		LCD5110_x++;
	}
	LCD5110_x++;
}

void LCD5110_Puts(char *c, LCD5110_Pixel_t color, LCD5110_FontSize_t size) {
	while (*c) {
		LCD5110_Putc(*c++, color, size);
	}
}