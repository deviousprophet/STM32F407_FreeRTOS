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
#define PIN_RST			GPIO_PIN_NO_9

#define PORT_ZX_IT		GPIOD
#define PIN_ZX_IT		GPIO_PIN_NO_11

#define PORT_SAG		GPIOD
#define PIN_SAG			GPIO_PIN_NO_13

#define PORT_IRQ_IT		GPIOD
#define PIN_IRQ_IT		GPIO_PIN_NO_15

// MODE Register
typedef enum {
	DISHPF,
	DISLPF2,
	DISCF,
	DISSAG,
	ASUSPEND,
	TEMPSEL,
	SWRST,
	CYCMODE,
	DISCH1,
	DISCH2,
	SWAP,
	DTRT0,
	DTRT1,
	WAVSEL0,
	WAVSEL1,
	POAM
} ADE_Mode_Reg_t;

typedef enum {
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
} ADE_IRQ_Reg_t;

typedef enum {
	BIT_SET,
	BIT_CLEAR
} ADE_Bit_State_t;

typedef enum {
	ADE_INT_ZX,
	ADE_INT_IRQ,
	ADE_INT_SAG
} ADE_INT_t;

void ADE_Init(void);
void ADE_Reset(void);
uint32_t ADE_ReadData(uint8_t addr, uint32_t bytes_to_read);
void ADE_WriteData(uint8_t address, uint32_t write_buffer, uint32_t bytes_to_write);
void ADE_MODE_Reg_Config(ADE_Mode_Reg_t bit_flag, ADE_Bit_State_t state);

#endif /* INC_ADE7753_H_ */
