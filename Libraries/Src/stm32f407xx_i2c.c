/*
 * stm32f407xx_i2c.c
 *
 *  Created on: Jun 12, 2021
 *      Author: deviousprophet
 */


#include "stm32f407xx_i2c.h"
#include "stm32f407xx_rcc.h"

void I2C_PeripheralControl(I2C_RegDef_t *I2Cx, uint8_t EnOrDi) {
	if(EnOrDi == ENABLE)
		I2Cx->CR1 |= (1 << I2C_CR1_PE);
		//pI2cBaseAddress->CR1 |= I2C_CR1_PE_Bit_Mask;
	else
		I2Cx->CR1 &= ~(1 << 0);
}

void I2C_PeriClockControl(I2C_RegDef_t *I2Cx, uint8_t EnorDi) {
	if(EnorDi == ENABLE) {
		if(I2Cx == I2C1)
			I2C1_PCLK_EN();
		else if (I2Cx == I2C2)
			I2C2_PCLK_EN();
		else if (I2Cx == I2C3)
			I2C3_PCLK_EN();
	} else {
		//TODO
	}
}

void I2C_Init(I2C_Handle_t *pI2CHandle) {
	uint32_t tempreg = 0 ;

	//enable the clock for the i2cx peripheral
	I2C_PeriClockControl(pI2CHandle->pI2Cx, ENABLE);

	I2C_PeripheralControl(pI2CHandle->pI2Cx, DISABLE);

	//ack control bit
	tempreg |= pI2CHandle->I2C_Config.I2C_AckControl << 10;
	pI2CHandle->pI2Cx->CR1 = tempreg;

	//configure the FREQ field of CR2
	tempreg = 0;
	tempreg |= RCC_GetPCLK1Value() /1000000U;
	pI2CHandle->pI2Cx->CR2 =  (tempreg & 0x3F);

   //program the device own address
	tempreg = 0;
	tempreg |= pI2CHandle->I2C_Config.I2C_DeviceAddress << 1;
	tempreg |= ( 1 << 14);
	pI2CHandle->pI2Cx->OAR1 = tempreg;

	//CCR calculations
	uint16_t ccr_value = 0;
	tempreg = 0;
	if(pI2CHandle->I2C_Config.I2C_SCLSpeed <= I2C_SCL_SPEED_SM) {
		//mode is standard mode
		ccr_value = (RCC_GetPCLK1Value() / ( 2 * pI2CHandle->I2C_Config.I2C_SCLSpeed ) );
		tempreg |= (ccr_value & 0xFFF);
	} else {
		//mode is fast mode
		tempreg |= ( 1 << 15);
		tempreg |= (pI2CHandle->I2C_Config.I2C_FMDutyCycle << 14);
		if(pI2CHandle->I2C_Config.I2C_FMDutyCycle == I2C_FM_DUTY_2)
			ccr_value = (RCC_GetPCLK1Value() / ( 3 * pI2CHandle->I2C_Config.I2C_SCLSpeed ) );
		else
			ccr_value = (RCC_GetPCLK1Value() / ( 25 * pI2CHandle->I2C_Config.I2C_SCLSpeed ) );
		tempreg |= (ccr_value & 0xFFF);
	}
	pI2CHandle->pI2Cx->CCR = tempreg;

	//TRISE Configuration
	if(pI2CHandle->I2C_Config.I2C_SCLSpeed <= I2C_SCL_SPEED_SM)
		//mode is standard mode
		tempreg = (RCC_GetPCLK1Value() /1000000U) + 1 ;
	else
		//mode is fast mode
		tempreg = ( (RCC_GetPCLK1Value() * 300) / 1000000000U ) + 1;

	pI2CHandle->pI2Cx->TRISE = (tempreg & 0x3F);

	I2C_PeripheralControl(pI2CHandle->pI2Cx, ENABLE);
}

uint8_t I2C_Read(I2C_RegDef_t *I2Cx, uint8_t address, uint8_t reg) {
	I2C_Start(I2Cx, address, I2C_TRANSMITTER_MODE, I2C_ACK_DISABLE);
	I2C_WriteData(I2Cx, reg);
	I2C_Stop(I2Cx);
	I2C_Start(I2Cx, address, I2C_RECEIVER_MODE, I2C_ACK_DISABLE);
	return I2C_ReadNack(I2Cx);
}

void I2C_ReadMulti(I2C_RegDef_t *I2Cx, uint8_t address, uint8_t reg, uint8_t *data, uint16_t count) {
	I2C_Start(I2Cx, address, I2C_TRANSMITTER_MODE, I2C_ACK_ENABLE);
	I2C_WriteData(I2Cx, reg);
	I2C_Start(I2Cx, address, I2C_RECEIVER_MODE, I2C_ACK_ENABLE);
	while(count--) {
		if(!count)
			*data++ = I2C_ReadNack(I2Cx);
		else
			*data++ = I2C_ReadAck(I2Cx);
	}
}

void I2C_Write(I2C_RegDef_t *I2Cx, uint8_t address, uint8_t reg, uint8_t data) {
	I2C_Start(I2Cx, address, I2C_TRANSMITTER_MODE, I2C_ACK_DISABLE);
	I2C_WriteData(I2Cx, reg);
	I2C_WriteData(I2Cx, data);
	I2C_Stop(I2Cx);
}

void I2C_WriteMulti(I2C_RegDef_t *I2Cx, uint8_t address, uint8_t reg, uint8_t *data, uint16_t count) {
	I2C_Start(I2Cx, address, I2C_TRANSMITTER_MODE, I2C_ACK_DISABLE);
	I2C_WriteData(I2Cx, reg);
	while(count--) I2C_WriteData(I2Cx, *data++);
	I2C_Stop(I2Cx);
}

uint8_t I2C_IsDeviceConnected(I2C_RegDef_t* I2Cx, uint8_t address) {
	uint8_t connected = 0;
	if(!I2C_Start(I2Cx, address, I2C_TRANSMITTER_MODE, I2C_ACK_ENABLE)) connected = 1;
	I2C_Stop(I2Cx);
	return connected;
}

int16_t I2C_Start(I2C_RegDef_t* I2Cx, uint8_t address, uint8_t direction, uint8_t ack) {
	I2Cx->CR1 |= (1 << I2C_CR1_START);

	uint32_t timeout = I2C_TIMEOUT;
	while(!(I2Cx->SR1 & (1 << I2C_SR1_SB)))
		if(--timeout == 0) return 1;

	if(ack) I2Cx->CR1 |= (1 << I2C_CR1_ACK);

	if(direction == I2C_TRANSMITTER_MODE)
		I2Cx->DR = address & ~(0x01);
	else I2Cx->DR = address | 0x01;

	timeout = I2C_TIMEOUT;
	while(!(I2Cx->SR1 & (1 << I2C_SR1_ADDR)))
		if(--timeout == 0) return 1;

	I2Cx->SR2;

	return 0;
}

uint8_t I2C_Stop(I2C_RegDef_t* I2Cx) {
	uint32_t timeout = I2C_TIMEOUT;
	while(!(I2Cx->SR1 & (1 << I2C_SR1_TXE)) || !(I2Cx->SR1 & (1 << I2C_SR1_BTF)))
		if(--timeout == 0) return 1;

	I2Cx->CR1 |= (1 << I2C_CR1_STOP);

	return 0;
}

uint8_t I2C_ReadNack(I2C_RegDef_t* I2Cx) {
	I2Cx->CR1 &= ~(1 << I2C_CR1_ACK);

	I2Cx->CR1 |= (1 << I2C_CR1_STOP);

	uint32_t timeout = I2C_TIMEOUT;
	while(!(I2Cx->SR1 & (1 << I2C_SR1_RXNE)))
		if(--timeout == 0) return 1;

	return I2Cx->DR;
}

uint8_t I2C_ReadAck(I2C_RegDef_t* I2Cx) {
	I2Cx->CR1 |= (1 << I2C_CR1_ACK);

	uint32_t timeout = I2C_TIMEOUT;
	while(!(I2Cx->SR1 & (1 << I2C_SR1_RXNE)))
		if(--timeout == 0) return 1;

	return I2Cx->DR;
}

void I2C_WriteData(I2C_RegDef_t* I2Cx, uint8_t data) {
	uint32_t timeout = I2C_TIMEOUT;
	while(!(I2Cx->SR1 & (1 << I2C_SR1_TXE)))
		if(--timeout == 0) break;
	I2Cx->DR = data;
}
