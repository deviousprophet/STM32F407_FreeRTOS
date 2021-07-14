/*
 * ds1307.c
 *
 *  Created on: Jun 12, 2021
 *      Author: deviousprophet
 */


#include "ds1307.h"

uint8_t DS1307_Bcd2Bin(uint8_t bcd);
uint8_t DS1307_Bin2Bcd(uint8_t bin);
uint8_t DS1307_CheckMinMax(uint8_t val, uint8_t min, uint8_t max);

DS1307_Result_t DS1307_Init(void) {
//	I2C GPIO Init
	GPIO_Handle_t ds_gpio;
	ds_gpio.GPIO_PinConfig.GPIO_PinAltFunMode = 4;
	ds_gpio.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	ds_gpio.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_OD;
	ds_gpio.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PIN_PU;
	ds_gpio.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_MEDIUM;

	ds_gpio.pGPIOx = DS1307_I2C_SCL_GPIO_PORT;
	ds_gpio.GPIO_PinConfig.GPIO_PinNumber = DS1307_I2C_SCL_GPIO_PIN;
	GPIO_Init(&ds_gpio);

	ds_gpio.pGPIOx = DS1307_I2C_SDA_GPIO_PORT;
	ds_gpio.GPIO_PinConfig.GPIO_PinNumber = DS1307_I2C_SDA_GPIO_PIN;
	GPIO_Init(&ds_gpio);

	I2C_Handle_t ds_i2c;
	ds_i2c.pI2Cx = DS1307_I2C;
	ds_i2c.I2C_Config.I2C_AckControl = I2C_ACK_DISABLE;
	ds_i2c.I2C_Config.I2C_SCLSpeed = DS1307_I2C_CLOCK;
	ds_i2c.I2C_Config.I2C_FMDutyCycle = I2C_FM_DUTY_2;

	I2C_Init(&ds_i2c);

	if(!I2C_IsDeviceConnected(DS1307_I2C, DS1307_I2C_ADDR)) return DS1307_Result_DeviceNotConnected;
	return DS1307_Result_OK;
}

uint8_t DS1307_GetSeconds(void) {
	return DS1307_Bcd2Bin(I2C_Read(DS1307_I2C, DS1307_I2C_ADDR, DS1307_SECONDS));
}

uint8_t DS1307_GetMinutes(void) {
	return DS1307_Bcd2Bin(I2C_Read(DS1307_I2C, DS1307_I2C_ADDR, DS1307_MINUTES));
}

uint8_t DS1307_GetHours(void) {
	return DS1307_Bcd2Bin(I2C_Read(DS1307_I2C, DS1307_I2C_ADDR, DS1307_HOURS));
}

uint8_t DS1307_GetDay(void) {
	return DS1307_Bcd2Bin(I2C_Read(DS1307_I2C, DS1307_I2C_ADDR, DS1307_DAY));
}

uint8_t DS1307_GetDate(void) {
	return DS1307_Bcd2Bin(I2C_Read(DS1307_I2C, DS1307_I2C_ADDR, DS1307_DATE));
}

uint8_t DS1307_GetMonth(void) {
	return DS1307_Bcd2Bin(I2C_Read(DS1307_I2C, DS1307_I2C_ADDR, DS1307_MONTH));
}

uint8_t DS1307_GetYear(void) {
	return DS1307_Bcd2Bin(I2C_Read(DS1307_I2C, DS1307_I2C_ADDR, DS1307_YEAR));
}

void DS1307_SetSeconds(uint8_t seconds) {
	I2C_Write(DS1307_I2C, DS1307_I2C_ADDR, DS1307_SECONDS, DS1307_Bin2Bcd(DS1307_CheckMinMax(seconds, 0, 59)));
}

void DS1307_SetMinutes(uint8_t minutes) {
	I2C_Write(DS1307_I2C, DS1307_I2C_ADDR, DS1307_MINUTES, DS1307_Bin2Bcd(DS1307_CheckMinMax(minutes, 0, 59)));
}

void DS1307_SetHours(uint8_t hours) {
	I2C_Write(DS1307_I2C, DS1307_I2C_ADDR, DS1307_HOURS, DS1307_Bin2Bcd(DS1307_CheckMinMax(hours, 0, 23)));
}

void DS1307_SetDay(uint8_t day) {
	I2C_Write(DS1307_I2C, DS1307_I2C_ADDR, DS1307_DAY, DS1307_Bin2Bcd(DS1307_CheckMinMax(day, 1, 7)));
}

void DS1307_SetDate(uint8_t date) {
	I2C_Write(DS1307_I2C, DS1307_I2C_ADDR, DS1307_DATE, DS1307_Bin2Bcd(DS1307_CheckMinMax(date, 1, 31)));
}

void DS1307_SetMonth(uint8_t month) {
	I2C_Write(DS1307_I2C, DS1307_I2C_ADDR, DS1307_MONTH, DS1307_Bin2Bcd(DS1307_CheckMinMax(month, 1, 12)));
}

void DS1307_SetYear(uint8_t year) {
	I2C_Write(DS1307_I2C, DS1307_I2C_ADDR, DS1307_YEAR, DS1307_Bin2Bcd(DS1307_CheckMinMax(year, 0, 99)));
}

void DS1307_GetDateTime(DS1307_DateTime_t* time) {
	time->seconds = DS1307_GetSeconds();
	time->minutes = DS1307_GetMinutes();
	time->hours = DS1307_GetHours();
	time->day = DS1307_GetDay();
	time->date = DS1307_GetDate();
	time->month = DS1307_GetMonth();
	time->year = DS1307_GetYear();
}

void DS1307_SetDateTime(DS1307_DateTime_t* time) {
	DS1307_SetSeconds(time->seconds);
	DS1307_SetMinutes(time->minutes);
	DS1307_SetHours(time->hours);
	DS1307_SetDay(time->day);
	DS1307_SetDate(time->date);
	DS1307_SetMonth(time->month);
	DS1307_SetYear(time->year);
}

void DS1307_EnableOutputPin(DS1307_OutputFreq_t frequency) {
	uint8_t temp;
	switch (frequency) {
		case DS1307_OutputFreq_1HZ:
			temp = DS1307_CONTROL_OUT_FLAG | DS1307_CONTROL_SQWE_FLAG;
			break;
		case DS1307_OutputFreq_4096HZ:
			temp = DS1307_CONTROL_OUT_FLAG | DS1307_CONTROL_SQWE_FLAG | DS1307_CONTROL_RS0_FLAG;
			break;
		case DS1307_OutputFreq_8192HZ:
			temp = DS1307_CONTROL_OUT_FLAG | DS1307_CONTROL_SQWE_FLAG | DS1307_CONTROL_RS1_FLAG;
			break;
		case DS1307_OutputFreq_32768HZ:
			temp = DS1307_CONTROL_OUT_FLAG | DS1307_CONTROL_SQWE_FLAG | DS1307_CONTROL_RS0_FLAG | DS1307_CONTROL_RS1_FLAG;
			break;
		case DS1307_OutputFreq_HIGH:
			temp = DS1307_CONTROL_OUT;
			break;
		case DS1307_OutputFreq_LOW:
			temp = 0;
			break;
		default:
			break;
	}

	I2C_Write(DS1307_I2C, DS1307_I2C_ADDR, DS1307_CONTROL, temp);
}

void DS1307_DisableOutputPin(void) {
	DS1307_EnableOutputPin(DS1307_OutputFreq_HIGH);
}

uint8_t DS1307_Bcd2Bin(uint8_t bcd) {
	return (bcd >> 4) * 10 + (bcd & 0x0F);
}

uint8_t DS1307_Bin2Bcd(uint8_t bin) {
	return (bin / 10) << 4 | bin % 10;
}

uint8_t DS1307_CheckMinMax(uint8_t val, uint8_t min, uint8_t max) {
	if(val < min)
		return min;
	else if (val > max)
		return max;
	else return val;
}
