/*
 * ade_value_scale.c
 *
 *  Created on: Jul 5, 2021
 *      Author: deviousprophet
 */


#include "ade_value_scale.h"

float power = 0, apparant_power = 0;

float ade_signed_value(uint32_t value, uint8_t bytes) {
	return 0;
}

float ade_scale_vrms(uint32_t value) {
	return value;
}

float ade_scale_vpeak(uint32_t value) {
	return value;
}

float ade_scale_irms(uint32_t value) {
	return value;
}

float ade_scale_ipeak(uint32_t value) {
	return value;
}

float ade_scale_power(uint32_t value) {
	return power = value;
}

float ade_scale_reactive_power(uint32_t value) {
	return value;
}

float ade_scale_apparant_power(uint32_t value) {
	return apparant_power = value;
}

float ade_get_power_factor() {
	if(power && apparant_power)
		return power/apparant_power;
	else return 0;
}

float ade_scale_energy(uint32_t value) {
	return value;
}

float ade_scale_apparant_energy(uint32_t value) {
	return value;
}
