/*
 * ade7753.h
 *
 *  Created on: Jun 12, 2021
 *      Author: deviousprophet
 */

#ifndef INC_ADE7753_H_
#define INC_ADE7753_H_

#include "stm32f407xx.h"
#include "stm32f407xx_gpio.h"
#include "stm32f407xx_spi.h"
#include <stdbool.h>

/*
 * ADE REGISTERS
 */
#define WAVEFORM   	0x01	//24bits
#define AENERGY     0x02	//24bits
#define RAENERGY    0x03	//24bits
#define LAENERGY    0x04	//24bits
#define VAENERGY    0x05	//24bits
#define RVAENERGY   0x06	//24bits
#define LVAENERGY   0x07	//24bits
#define LVARENERGY  0x08	//24bits

#define MODE        0x09
#define IRQEN       0x0A
#define STATUS      0x0B
#define RSTSTATUS   0x0C

#define CH1OS       0x0D
#define CH2OS       0x0E
#define GAIN        0x0F
#define PHCAL       0x10

#define APOS        0x11
#define WGAIN       0x12	//12bits
#define WDIV        0x13
#define CFNUM       0x14 	//12bits
#define CFDEN       0x15
#define IRMS        0x16
#define VRMS        0x17
#define IRMSOS      0x18
#define VRMSOS      0x19
#define VAGAIN      0x1A
#define VADIV       0x1B
#define LINECYC     0x1C
#define ZXTOUT      0x1D
#define SAGCYC      0x1E
#define SAGLVL      0x1F
#define IPKLVL      0x20
#define VPKLVL      0x21
#define IPEAK       0x22
#define RSTIPEAK    0x23
#define VPEAK       0x24
#define RSTVPEAK    0x25
#define TEMP        0x26
#define PERIOD      0x27

#define TMODE       0x3D
#define CHKSUM      0x3E
#define DIEREV      0x3F

/*
 * Pin Mode
 */
#define ADE_SPI_HOST	SPI2
#define SPI_PORT 		GPIOB
#define SPI_PIN_MOSI 	GPIO_PIN_NO_15
#define SPI_PIN_MISO	GPIO_PIN_NO_14
#define SPI_PIN_SCLK	GPIO_PIN_NO_13
#define SPI_PIN_NSS		GPIO_PIN_NO_12

//ADE Pin
#define PORT_RST		GPIOD
#define PIN_RST			GPIO_PIN_NO_15

#define PORT_ZX_IT		GPIOD
#define PIN_ZX_IT		GPIO_PIN_NO_13

#define PORT_SAG_IT		GPIOD
#define PIN_SAG_IT		GPIO_PIN_NO_11

#define PORT_IRQ_IT		GPIOD
#define PIN_IRQ_IT		GPIO_PIN_NO_9

// MODE Register
enum {
	MODE_DISHPF,
	MODE_DISLPF2,
	MODE_DISCF,
	MODE_DISSAG,
	MODE_ASUSPEND,
	MODE_TEMPSEL,
	MODE_SWRST,
	MODE_CYCMODE,
	MODE_DISCH1,
	MODE_DISCH2,
	MODE_SWAP,
	MODE_DTRT0,
	MODE_DTRT1,
	MODE_WAVSEL0,
	MODE_WAVSEL1,
	MODE_POAM
} ADE_Mode_Reg;

// IRQ Register
enum {
	IRQ_AEHF,
	IRQ_SAG,
	IRQ_CYCEND,
	IRQ_WSMP,
	IRQ_ZX,
	IRQ_TEMP,
	IRQ_RESET,
	IRQ_AEOF,
	IRQ_PKV,
	IRQ_PKI,
	IRQ_VAEHF,
	IRQ_VAEOF,
	IRQ_ZXTO,
	IRQ_PPOS,
	IRQ_PNEG
} ADE_IRQ_Reg;

typedef enum {
	ADE_INT_ZX,
	ADE_INT_IRQ,
	ADE_INT_SAG,
	ADE_USER_CONFIG
} ADE_Event_t;

typedef enum {
	GAIN_1,
	GAIN_2,
	GAIN_4,
	GAIN_8,
	GAIN_16
} PGA_GAIN_t;

typedef enum {
	FULL_SCALE_05,
	FULL_SCALE_025,
	FULL_SCALE_0125
} CH1_Full_Scale_Select_t;

void ADE_Init(void);
void ADE_Reset(void);
uint32_t ADE_ReadData(uint8_t addr, uint8_t bytes_to_read);
void ADE_WriteData(uint8_t address, uint32_t write_buffer, uint8_t bytes_to_write);
void ADE_SetGain(CH1_Full_Scale_Select_t ch1_full_scale, PGA_GAIN_t ch1_gain, PGA_GAIN_t ch2_gain);

#endif /* INC_ADE7753_H_ */
