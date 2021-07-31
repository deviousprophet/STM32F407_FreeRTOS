/*
 * ade_value_scale.c
 *
 *  Created on: Jul 5, 2021
 *      Author: deviousprophet
 */


#include "ade_value_scale.h"

float power = 0, reactive_power = 0, apparant_power = 0;
float energy = 0;

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
	return ((float) value) * VRMS_SCALE_CONST;
}

float ade_scale_irms(uint32_t value) {
	return ((float) value) * IRMS_SCALE_CONST;
}

float ade_scale_power(uint32_t value) {
	return power = ((float) ade_signed_value(value, 23)) * POWER_SCALE_CONST;
}

float ade_scale_reactive_power(uint32_t value) {
	return reactive_power = ((float) ade_signed_value(value, 23)) * REACTIVE_POWER_SCALE_CONST;
}

float ade_get_apparant_power() {
	return apparant_power = sqrtf(power*power + reactive_power*reactive_power);
}

float ade_get_power_factor() {
	if(power) return power / apparant_power;
	return 0;
}

float ade_accumulate_scale_energy(uint32_t value) {
	return energy += ((float) ade_signed_value(value, 23)) * ENERGY_SCALE_CONST;
}

void reset_energy(void) {
	energy = 0;
}

float rescale_hex_to_user_pkv(uint32_t value) {
	value <<= 7;
	float temp = value;
	temp /= MAX_VRMS_INPUT_SAMPLE / (VRMS_SCALE_CONST * 1561400);
	return temp;
}

float rescale_hex_to_user_pki(uint32_t value) {
	value <<= 15;
	float temp = value;
	temp /= MAX_IRMS_INPUT_SAMPLE / (IRMS_SCALE_CONST * 1868467);
	return temp;
}

float rescale_hex_to_user_sag(uint32_t value) {
	value <<= 7;
	float temp = value;
	temp /= MAX_VRMS_INPUT_SAMPLE / (VRMS_SCALE_CONST * 1561400);
	return temp;
}

uint32_t rescale_user_pkv_to_hex(float value) {
	value *= MAX_VRMS_INPUT_SAMPLE / (VRMS_SCALE_CONST * 1561400);
	uint32_t temp = (uint32_t) value;
	temp >>= 7;
	return temp;
}

uint32_t rescale_user_pki_to_hex(float value) {
	value *= MAX_IRMS_INPUT_SAMPLE / (IRMS_SCALE_CONST * 1868467);
	uint32_t temp = (uint32_t) value;
	temp >>= 15;
	return temp;
}

uint32_t rescale_user_sag_to_hex(float value) {
	value *= MAX_VRMS_INPUT_SAMPLE / (VRMS_SCALE_CONST * 1561400);
	uint32_t temp = (uint32_t) value;
	if(temp & (1 << 6)) temp += 1 << 6;
	temp >>= 7;
	return temp;
}
