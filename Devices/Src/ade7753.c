/*
 * ade7753.c
 *
 *  Created on: Jun 12, 2021
 *      Author: deviousprophet
 */


#include "ade7753.h"

void ADE_SPI_Init() {
	/*
	 * PB14 --> SPI2_MISO
	 * PB15 --> SPI2_MOSI
	 * PB13 -> SPI2_SCLK
	 * PB12 --> SPI2_NSS
	 * ALT function mode : 5
	 */
	GPIO_Handle_t SPIPins;
	SPIPins.pGPIOx = GPIOB;
	SPIPins.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	SPIPins.GPIO_PinConfig.GPIO_PinAltFunMode = 5;
	SPIPins.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
	SPIPins.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;
	SPIPins.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;

	//SCLK
	SPIPins.GPIO_PinConfig.GPIO_PinNumber = 13;
	GPIO_Init(&SPIPins);

	//MOSI
	SPIPins.GPIO_PinConfig.GPIO_PinNumber = 15;
	GPIO_Init(&SPIPins);

	//MISO
	SPIPins.GPIO_PinConfig.GPIO_PinNumber = 14;
	GPIO_Init(&SPIPins);

	//NSS
	SPIPins.GPIO_PinConfig.GPIO_PinNumber = 12;
	GPIO_Init(&SPIPins);

	//RESET Pin GPIO
	SPIPins.pGPIOx = PORT_RST;
	SPIPins.GPIO_PinConfig.GPIO_PinNumber = PIN_RST;
	SPIPins.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	SPIPins.GPIO_PinConfig.GPIO_PinAltFunMode = 0;
	GPIO_Init(&SPIPins);

	SPI_Handle_t SPI2handle;
	SPI2handle.pSPIx = ADE_SPI_HOST;
	SPI2handle.SPIConfig.SPI_BusConfig = SPI_BUS_CONFIG_FD;
	SPI2handle.SPIConfig.SPI_DeviceMode = SPI_DEVICE_MODE_MASTER;
	SPI2handle.SPIConfig.SPI_SclkSpeed = SPI_SCLK_SPEED_DIV16;	// clock 1MHz
	SPI2handle.SPIConfig.SPI_DFF = SPI_DFF_8BITS;
	SPI2handle.SPIConfig.SPI_CPOL = SPI_CPOL_LOW;				// CPOL 0
	SPI2handle.SPIConfig.SPI_CPHA = SPI_CPHA_HIGH;				// CPHA 1
	SPI2handle.SPIConfig.SPI_SSM = SPI_SSM_DI; 					// Hardware slave management enabled for NSS pin

	ADE_SPI_HOST->CR1 &= ~(1 << SPI_CR1_SPE);
	SPI_Init(&SPI2handle);
	ADE_SPI_HOST->CR1 |= (1 << SPI_CR1_SPE);

	SPI_SSOEConfig(ADE_SPI_HOST, ENABLE);
}

void ZeroX_Init() {
//	Input pin trigger as falling edge
	GPIO_Handle_t GpioZX;
	GpioZX.pGPIOx = PORT_ZX_IT;
	GpioZX.GPIO_PinConfig.GPIO_PinNumber = PIN_ZX_IT;
	GpioZX.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IT_FT;
	GpioZX.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
	GpioZX.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;
	GPIO_Init(&GpioZX);
}

void IRQ_Init() {
//	Input pin trigger as falling edge
	GPIO_Handle_t GpioIRQ;
	GpioIRQ.pGPIOx = PORT_IRQ_IT;
	GpioIRQ.GPIO_PinConfig.GPIO_PinNumber = PIN_IRQ_IT;
	GpioIRQ.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IT_FT;
	GpioIRQ.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
	GpioIRQ.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PIN_PU;
	GPIO_Init(&GpioIRQ);
}

void SAG_Init() {
//	Input pin trigger as falling edge
	GPIO_Handle_t GpioSAG;
	GpioSAG.pGPIOx = PORT_SAG_IT;
	GpioSAG.GPIO_PinConfig.GPIO_PinNumber = PIN_SAG_IT;
	GpioSAG.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IT_RFT;
	GpioSAG.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
	GpioSAG.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PIN_PU;
	GPIO_Init(&GpioSAG);
}

void ADE_Init(void) {
	ADE_SPI_Init();
	ZeroX_Init();
//	SAG_Init();
	IRQ_Init();

//	IRQ configuration
	GPIO_IRQPriorityConfig(IRQ_NO_EXTI15_10, NVIC_IRQ_PRI15);
	GPIO_IRQInterruptConfig(IRQ_NO_EXTI15_10, ENABLE);

	ADE_Reset();
}

void ADE_Reset(void) {
	GPIO_WriteToOutputPin(PORT_RST, PIN_RST, 0);
	for(int i = 0; i < 1000; i++);
	GPIO_WriteToOutputPin(PORT_RST, PIN_RST, 1);
}

uint32_t ADE_ReadData(uint8_t address, uint8_t bytes_to_read) {
	uint32_t data = 0;
	SPI_PeripheralControl(ADE_SPI_HOST, ENABLE); //SS pin pull to low
	SPI_Transfer(ADE_SPI_HOST, address);
	for(uint8_t i = 0; i < bytes_to_read; i++) {
		data <<= 8;
		data |= SPI_Transfer(ADE_SPI_HOST, 0xff);
	}
	SPI_PeripheralControl(ADE_SPI_HOST, DISABLE); //SS pin pull to high
	return data;
}

void ADE_WriteData(uint8_t address, uint32_t write_buffer, uint8_t bytes_to_write) {
	uint8_t data = 0;
	address |= 0x80;
	SPI_PeripheralControl(ADE_SPI_HOST, ENABLE); //SS pin pull to low
	SPI_Transfer(ADE_SPI_HOST, address);
	for(uint8_t i = 0; i < bytes_to_write; i++) {
		data = (uint8_t) (write_buffer >> 8 * (bytes_to_write - i - 1));
		SPI_Transfer(ADE_SPI_HOST, data);
	}
	SPI_PeripheralControl(ADE_SPI_HOST, DISABLE);; //SS pin pull to high
}

void ADE_SetGain(CH1_Full_Scale_Select_t ch1_full_scale, PGA_GAIN_t ch1_gain, PGA_GAIN_t ch2_gain) {
	ADE_WriteData(GAIN, 0x00 | (ch2_gain << 5) | (ch1_full_scale << 3) | ch1_gain, 1);
}
