/*
 * ade_value_scale.c
 *
 *  Created on: Jul 5, 2021
 *      Author: deviousprophet
 */


#include "ade_value_scale.h"

float power = 0, apparant_power = 0;

int ade_signed_value(uint32_t value, uint8_t msb) {
	int val = value;
	if(val & (1 << msb)) {
		for(int i = 0; i <= msb; i++)
			val ^= (1 << i);
		val++;
		val *= -1;
	}
	return val;
}

float ade_scale_vrms(uint32_t value) {
	return value;
}

float ade_scale_vpeak(uint32_t value) {
	return value;
}

float ade_scale_irms(uint32_t value) {
	return ((float) value) / 764.9270602;
}

float ade_scale_ipeak(uint32_t value) {
	return value;
}

float ade_scale_power(uint32_t value) {
	return power = ((float) ade_signed_value(value, 23)) * 26.95655;
}

float ade_scale_reactive_power(uint32_t value) {
	return ((float) ade_signed_value(value, 23)) * 71.27522;
}

float ade_scale_apparant_power(uint32_t value) {
	return apparant_power = ((float) value) * 30.42205;
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
