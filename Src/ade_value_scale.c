/*
 * ade_value_scale.c
 *
 *  Created on: Jul 5, 2021
 *      Author: deviousprophet
 */


#include "ade_value_scale.h"

float power = 0, reactive_power = 0, apparant_power = 0;

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
	return ((float) value) / 3.082983;
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
	return power = ((float) ade_signed_value(value, 23)) * 27.4245168;
}

float ade_scale_reactive_power(uint32_t value) {
	return reactive_power = ((float) ade_signed_value(value, 23)) * 77.14371603;
}

float ade_get_apparant_power() {
	return apparant_power = sqrtf(power*power + reactive_power*reactive_power);
}

float ade_get_power_factor() {
	if(power) return power / apparant_power;
	return 0;
}

float ade_scale_energy(uint32_t value) {
	return value;
}

float ade_scale_apparant_energy(uint32_t value) {
	return value;
}
