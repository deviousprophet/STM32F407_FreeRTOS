/*
 * ade_value_scale.h
 *
 *  Created on: Jul 5, 2021
 *      Author: deviousprophet
 */

#ifndef ADE_VALUE_SCALE_H_
#define ADE_VALUE_SCALE_H_

#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include "ade7753.h"
#include "lcd_control.h"
#include "ds1307.h"

//#define ENERGY_UNIT_WS					1

//#define VRMS_SCALE_CONST				0.324361179
//#define IRMS_SCALE_CONST				0.002614628
//#define POWER_SCALE_CONST				27.42451681
//#define REACTIVE_POWER_SCALE_CONST		77.14371603

#define VRMS_SCALE_CONST				0.324853531
#define IRMS_SCALE_CONST				0.002577779
#define POWER_SCALE_CONST				57.25325556
#define REACTIVE_POWER_SCALE_CONST		146.6178354

#ifndef ENERGY_UNIT_WS
#define ENERGY_SCALE_CONST				(POWER_SCALE_CONST / (4 * 3600))
#else
#define ENERGY_SCALE_CONST				POWER_SCALE_CONST / 4
#endif

#define MAX_VRMS_INPUT_SAMPLE			0x2518
#define MAX_IRMS_INPUT_SAMPLE			0x2851EC

/* Data Payload offset */
#define VRMS_OFFSET					0
#define VPEAK_OFFSET				4
#define IRMS_OFFSET					8
#define IPEAK_OFFSET				12
#define POWER_OFFSET				16
#define REACTIVEPOWER_OFFSET		20
#define APPARENTPOWER_OFFSET		24
#define POWERFACTOR_OFFSET			28
#define ENERGY_OFFSET				32
#define DAYS_OFFSET					36
#define HOUR_OFFSET					40
#define MINUTE_OFFSET				41
#define SECOND_OFFSET				42
#define PKV_OFFSET					43
#define PKI_OFFSET					47
#define SAG_OFFSET					51
#define INTERVAL_OFFSET				55
#define RTC_OFFSET					56

int ade_signed_value(uint32_t value, uint8_t msb);

float ade_scale_vrms(uint32_t value);
float ade_scale_irms(uint32_t value);

float ade_scale_power(uint32_t value);
float ade_scale_reactive_power(uint32_t value);
float ade_get_apparant_power();
float ade_get_power_factor();

float ade_accumulate_scale_energy(uint32_t value);
void reset_energy();

float rescale_hex_to_user_pkv(uint32_t value);
float rescale_hex_to_user_pki(uint32_t value);
float rescale_hex_to_user_sag(uint32_t value);

uint32_t rescale_user_pkv_to_hex(float value);
uint32_t rescale_user_pki_to_hex(float value);
uint32_t rescale_user_sag_to_hex(float value);

typedef struct {
	LCD_Data_Screen1_t data1;
	LCD_Data_Screen2_t data2;
	LCD_Data_Screen3_t data3;
	LCD_Data_Screen4_t data4;
	DS1307_DateTime_t datetime;
} Monitor_Data_t;

#define FRAME_UART_PREFIX				0xADEADE
#define FRAME_MONITOR_BUFFER_SIZE		68

void ade_data_to_frame_uart_send(Monitor_Data_t monitor_data, uint8_t* buffer);

#endif /* ADE_VALUE_SCALE_H_ */
