/*
 * ds1307.h
 *
 *  Created on: Jun 12, 2021
 *      Author: deviousprophet
 */

#ifndef DS1307_H_
#define DS1307_H_

#include "stm32f407xx.h"
#include "stm32f407xx_gpio.h"
#include "stm32f407xx_i2c.h"
#include <stdint.h>
#include <string.h>

#define DS1307_I2C					I2C3

#define DS1307_I2C_SCL_GPIO_PORT	GPIOA
#define DS1307_I2C_SCL_GPIO_PIN		GPIO_PIN_NO_8

#define DS1307_I2C_SDA_GPIO_PORT	GPIOC
#define DS1307_I2C_SDA_GPIO_PIN		GPIO_PIN_NO_9

#define DS1307_I2C_CLOCK			I2C_SCL_SPEED_SM

#define DS1307_I2C_ADDR				0xD0

#define DS1307_SECONDS				0x00
#define DS1307_MINUTES				0x01
#define DS1307_HOURS				0x02
#define DS1307_DAY					0x03
#define DS1307_DATE					0x04
#define DS1307_MONTH				0x05
#define DS1307_YEAR					0x06
#define DS1307_CONTROL				0x07

#define DS1307_CONTROL_OUT			7
#define DS1307_CONTROL_SQWE			4
#define DS1307_CONTROL_RS1			1
#define DS1307_CONTROL_RS0			0

#define DS1307_CONTROL_OUT_FLAG		(1 << DS1307_CONTROL_OUT)
#define DS1307_CONTROL_SQWE_FLAG	(1 << DS1307_CONTROL_SQWE)
#define DS1307_CONTROL_RS1_FLAG		(1 << DS1307_CONTROL_RS1)
#define DS1307_CONTROL_RS0_FLAG		(1 << DS1307_CONTROL_RS0)

#define SUNDAY				1
#define MONDAY				2
#define TUESDAY				3
#define WEDNESDAY			4
#define THURSDAY			5
#define FRIDAY				6
#define SATURDAY			7

typedef enum {
	DS1307_Result_OK,
	DS1307_Result_Error,
	DS1307_Result_DeviceNotConnected
} DS1307_Result_t;

typedef enum {
	DS1307_OutputFreq_1HZ,
	DS1307_OutputFreq_4096HZ,
	DS1307_OutputFreq_8192HZ,
	DS1307_OutputFreq_32768HZ,
	DS1307_OutputFreq_HIGH,
	DS1307_OutputFreq_LOW,
} DS1307_OutputFreq_t;

typedef struct {
	uint8_t seconds;
	uint8_t minutes;
	uint8_t hours;
	uint8_t day;
	uint8_t date;
	uint8_t month;
	uint8_t year;
} DS1307_DateTime_t;

/**
 * @brief  Initializes DS1307 RTC library
 * @param  None
 * @retval None
 */
DS1307_Result_t DS1307_Init(void);

/**
 * @brief  Gets seconds from DS1307
 * @param  None
 * @retval Seconds from DS1307, 00 to 59
 */
uint8_t DS1307_GetSeconds(void);

/**
 * @brief  Gets minutes from DS1307
 * @param  None
 * @retval Minutes from DS1307, 00 to 59
 */
uint8_t DS1307_GetMinutes(void);

/**
 * @brief  Gets hours from DS1307
 * @param  None
 * @retval Hours from DS1307, 00 to 23
 */
uint8_t DS1307_GetHours(void);

/**
 * @brief  Gets day in a week from DS1307
 * @param  None
 * @retval Day in a week from DS1307, 1 to 7
 */
uint8_t DS1307_GetDay(void);

/**
 * @brief  Gets date from DS1307
 * @param  None
 * @retval Date from DS1307, 01 to 31
 */
uint8_t DS1307_GetDate(void);

/**
 * @brief  Gets month from DS1307
 * @param  None
 * @retval Month from DS1307, 01 to 12
 */
uint8_t DS1307_GetMonth(void);

/**
 * @brief  Gets year from DS1307
 * @param  None
 * Returns year from DS1307, 00 (2000) to 99 (2099)
 */
uint8_t DS1307_GetYear(void);


/**
 * @brief  Sets seconds to DS1307
 * @param  seconds: Seconds to be set to DS1307, 01 to 59
 * @retval None
 */
void DS1307_SetSeconds(uint8_t seconds);

/**
 * @brief  Sets minutes to DS1307
 * @param  minutes: Minutes to be set to DS1307, 01 to 59
 * @retval None
 */
void DS1307_SetMinutes(uint8_t minutes);

/**
 * @brief  Sets hours to DS1307
 * @param  hours: Hours to be set to DS1307, 01 to 23
 * @retval None
 */
void DS1307_SetHours(uint8_t hours);

/**
 * @brief  Sets day to DS1307
 * @param  day: Day to be set to DS1307, 1 to 7
 * @retval None
 */
void DS1307_SetDay(uint8_t day);

/**
 * @brief  Sets date to DS1307
 * @param  date: Date to be set to DS1307, 01 to 31
 * @retval None
 */
void DS1307_SetDate(uint8_t date);

/**
 * @brief  Sets month to DS1307
 * @param  month: Month to be set to DS1307, 01 to 12
 * @retval None
 */
void DS1307_SetMonth(uint8_t month);

/**
 * @brief  Sets year to DS1307
 * @param  year: Year to be set to DS1307, 00 (2000) to 99 (2099)
 * @retval None
 */
void DS1307_SetYear(uint8_t year);

/**
 * @brief  Gets full date and time from DS1307
 * @param  *time: Pointer to @ret DS1307_Time_t structure where to set data
 * @retval None
 */
void DS1307_GetDateTime(DS1307_DateTime_t* time);

/**
 * @brief  Sets full date and time to DS1307
 * @param  *time: Pointer to @ret DS1307_Time_t structure where to get data
 * @retval None
 */
void DS1307_SetDateTime(DS1307_DateTime_t* time);

/**
 * @brief  DS1307 has SQW/OUT pin, which can be enabled in various modes.
 *         It can output 32768Hz, 8192Hz, 4096Hz, 1Hz, Low or High state.
 *         This is useful if you need interrupts on MCU. 1Hz can be used to increment time by software each time.
 *         This is faster than look for date and time each time.
 *
 * @note   Also, this pin is Open-Drain. This means that pin cannot supply positive power supply,
 *         for that you need external pull up resistor (or pull up from MCU).
 *
 * @param  frequency: SQW frequency. This parameter can be a value of @ref DS1307_OutputFrequency_t enumeration
 * @retval None
 */
void DS1307_EnableOutputPin(DS1307_OutputFreq_t frequency);

/**
 * @brief  Disables SQW/OUT pin
 *
 * This function basically set pin to high state.
 * To get high state you need external pull up resistor (or use pull up from MCU)
 * @retval None
 */
void DS1307_DisableOutputPin(void);

#endif /* DS1307_H_ */
