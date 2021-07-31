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

//#define ENERGY_UNIT_WS					1

#define VRMS_SCALE_CONST				1 / 3.082983
#define IRMS_SCALE_CONST				1 / 764.9270602
#define POWER_SCALE_CONST				27.4245168
#define REACTIVE_POWER_SCALE_CONST		77.14371603
#ifndef ENERGY_UNIT_WS
#define ENERGY_SCALE_CONST				(POWER_SCALE_CONST / (2 * 3600))
#else
#define ENERGY_SCALE_CONST				POWER_SCALE_CONST / 2
#endif

#define MAX_VRMS_INPUT_SAMPLE			0x2518
#define MAX_IRMS_INPUT_SAMPLE			0x2851EC

int ade_signed_value(uint32_t value, uint8_t msb);

float ade_scale_vrms(uint32_t value);
float ade_scale_irms(uint32_t value);

float ade_scale_power(uint32_t value);
float ade_scale_reactive_power(uint32_t value);
float ade_get_apparant_power();
float ade_get_power_factor();

float ade_accumulate_scale_energy(uint32_t value);
void reset_energy(void);

float rescale_hex_to_user_pkv(uint32_t value);
float rescale_hex_to_user_pki(uint32_t value);
float rescale_hex_to_user_sag(uint32_t value);

uint32_t rescale_user_pkv_to_hex(float value);
uint32_t rescale_user_pki_to_hex(float value);
uint32_t rescale_user_sag_to_hex(float value);

#endif /* ADE_VALUE_SCALE_H_ */
