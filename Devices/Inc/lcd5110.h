/*
 * lcd5110.h
 *
 *  Created on: Jun 12, 2021
 *      Author: deviousprophet
 */

#ifndef INC_LCD5110_H_
#define INC_LCD5110_H_

#include "stm32f407xx.h"
#include "stm32f407xx_gpio.h"
#include "stm32f407xx_spi.h"


//SPI used
#ifndef LCD5110_SPI
#define LCD5110_SPI				SPI1
#endif

//Default pins used
//Default RST pin
#ifndef LCD5110_RST_PIN
#define LCD5110_RST_PORT		GPIOE
#define LCD5110_RST_PIN			GPIO_PIN_NO_7
#endif
//Default DC pin
#ifndef LCD5110_DC_PIN
#define LCD5110_DC_PORT			GPIOC
#define LCD5110_DC_PIN			GPIO_PIN_NO_5
#endif

//Default CE pin
#ifndef LCD5110_CE_PIN
#define LCD5110_CE_PORT			GPIOB
#define LCD5110_CE_PIN			GPIO_PIN_NO_1
#endif



/*
 * Private enums
 */

typedef enum {
	LCD5110_COMMAND = 0,
	LCD5110_DATA = !LCD5110_COMMAND
} LCD5110_WriteType_t;

typedef enum {
	LCD5110_State_Low = 0,
	LCD5110_State_High = !LCD5110_State_Low
} LCD5110_State_t;

typedef enum {
	LCD5110_Pin_DC = 1,
	LCD5110_Pin_RST = 2
} LCD5110_Pin_t;

/*
 * Public enums
 */

/*
 * Used for pixel "color"
 * LCD is 2 bit, so pixel can be just set or clear
 */
typedef enum {
	LCD5110_Pixel_Clear = 0,
	LCD5110_Pixel_Set = !LCD5110_Pixel_Clear
} LCD5110_Pixel_t;

/*
 * Font size for text
 * There are 2 sizes included
 */
typedef enum {
	LCD5110_FontSize_5x7 = 0,
	LCD5110_FontSize_3x5 = !LCD5110_FontSize_5x7
} LCD5110_FontSize_t;

/*
 * Used to invert pixels
 */
typedef enum {
	LCD5110_Invert_Yes,
	LCD5110_Invert_No
} LCD5110_Invert_t;


#define LCD5110_WIDTH				84
#define LCD5110_HEIGHT				48

// LCD5110 Commandset
// ------------------
// General commands
#define LCD5110_POWERDOWN			0x04
#define LCD5110_ENTRYMODE			0x02
#define LCD5110_EXTENDEDINSTRUCTION	0x01
#define LCD5110_DISPLAYBLANK		0x00
#define LCD5110_DISPLAYNORMAL		0x04
#define LCD5110_DISPLAYALLON		0x01
#define LCD5110_DISPLAYINVERTED		0x05
// Normal instruction set
#define LCD5110_FUNCTIONSET			0x20
#define LCD5110_DISPLAYCONTROL		0x08
#define LCD5110_SETYADDR			0x40
#define LCD5110_SETXADDR			0x80
// Extended instruction set
#define LCD5110_SETTEMP				0x04
#define LCD5110_SETBIAS				0x10
#define LCD5110_SETVOP				0x80
// Display presets
#define LCD5110_LCD_BIAS			0x03	// Range: 0-7 (0x00-0x07)
#define LCD5110_LCD_TEMP			0x02	// Range: 0-3 (0x00-0x03)
#define LCD5110_LCD_CONTRAST		0x46	// Range: 0-127 (0x00-0x7F)

#define LCD5110_CHAR5x7_WIDTH		6 //5*8
#define LCD5110_CHAR5x7_HEIGHT		8
#define LCD5110_CHAR3x5_WIDTH		4 //3*5
#define LCD5110_CHAR3x5_HEIGHT		6

#define LCD5110_BUFFER_SIZE 		LCD5110_WIDTH * LCD5110_HEIGHT / 8

/*
 * this function is used to initialize the GPIO pins to behave as SPI2 pins
 */
void SPI1_GPIOInits(void);

/*
 * This function is used to initialize the SPI2 peripheral parameters
 */
void SPI1_Inits(void);

/*
 * Send data to LCD
 *
 */
extern void LCD5110_send(unsigned char data);

/**
 * Set DC pin state
 *
 */
extern void LCD5110_Pin(LCD5110_Pin_t pin, LCD5110_State_t state);

/**
 * Specify delay in microseconds
 *
 */
extern void LCD5110_Delay(unsigned long micros);

/**
 * Write data/command to LCD
 *
 */
extern void LCD5110_Write(LCD5110_WriteType_t cd, unsigned char data);

/**
 * Set area for refresh display
 *
 */
extern void LCD5110_UpdateArea(unsigned char xMin, unsigned char yMin, unsigned char xMax, unsigned char yMax);

/**
 * Initialize LCD
 *
 * Parameters
 * - unsigned char contrast: Contrast can be software selectable, from 0x00 to 0x7F
 */
extern void LCD5110_Init(unsigned char contrast);

/**
 * Set contrast for LCD
 *
 * Parameters
 * - unsigned char contrast: Contrast can be software selectable, from 0x00 to 0x7F
 */
extern void LCD5110_SetContrast(unsigned char contrast);

/**
 * Invert LCD
 *
 * Parameters
 * - LCD5110_Invert_t invert
 * 		- LCD5110_Invert_Yes: Invert pixels on lcd
 * 		- LCD5110_Invert_No: Return pixels to orignal
 */
extern void LCD5110_Invert(LCD5110_Invert_t invert);

/**
 * Clear lcd
 */
extern void LCD5110_Clear(void);

/**
 * Set/clear pixel at specific location
 *
 * Parameters
 * - unsigned char x: x position of pixel
 * - unsigned char y: y position of pixel
 * - LCD5110_LCD5110_Pixel_t pixel: Set or clear pixel
 * 		- LCD5110_Pixel_Set
 * 		- LCD5110_Pixel_Clear
 */
extern void LCD5110_DrawPixel(unsigned char x, unsigned char y, LCD5110_Pixel_t pixel);


/**
 * Set cursor on lcd at 0, 0 location
 *
 */
extern void LCD5110_Home(void);

/**
 * Put data from internal buffer to lcd
 *
 */
extern void LCD5110_Refresh(void);

/**
 * Set cursor to desired position
 *
 * Parameters:
 * - unsigned char x: x from 0 to 83
 * - unsigned char y: y from 0 to 47
 */
extern void LCD5110_GotoXY(unsigned char x, unsigned char y);

/**
 * Put character on LCD
 *
 * Parameters:
 * - char c: char to display
 * - LCD5110_FontSize_t size: Font size
 * 		- LCD5110_FontSize_5x7
 * 		- LCD5110_FontSize_3x5
 * - LCD5110_LCD5110_Pixel_t color
 * 		- LCD5110_Pixel_Set
 * 		- LCD5110_Pixel_Clear
 */
extern void LCD5110_Putc(char c, LCD5110_Pixel_t color, LCD5110_FontSize_t size);

/**
 * Put string on LCD
 *
 * Parameters:
 * - char *c: pointer to first character of string
 * - LCD5110_FontSize_t size: Font size
 * 		- LCD5110_FontSize_5x7
 * 		- LCD5110_FontSize_3x5
 * - LCD5110_LCD5110_Pixel_t color
 * 		- LCD5110_Pixel_Set
 * 		- LCD5110_Pixel_Clear
 */
extern void LCD5110_Puts(char *c, LCD5110_Pixel_t color, LCD5110_FontSize_t size);



#endif /* INC_LCD5110_H_ */
