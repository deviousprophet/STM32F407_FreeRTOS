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

int ade_signed_value(uint32_t value, uint8_t msb);

float ade_scale_vrms(uint32_t value);
float ade_scale_vpeak(uint32_t value);

float ade_scale_irms(uint32_t value);
float ade_scale_ipeak(uint32_t value);

float ade_scale_power(uint32_t value);
float ade_scale_reactive_power(uint32_t value);
float ade_get_apparant_power();
float ade_get_power_factor();

float ade_scale_energy(uint32_t value);
float ade_scale_apparant_energy(uint32_t value);

#endif /* ADE_VALUE_SCALE_H_ */
