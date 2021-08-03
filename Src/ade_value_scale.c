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

void reset_energy() {
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

void u8_from_float(uint8_t* buffer, float value) {
	uint32_t temp;
	memcpy(&temp, &value, sizeof(uint32_t));
	for(int i = 0; i < 4; i++) {
        buffer[3 - i] = temp & 0xFF;
        temp >>= 8;
    }
}

void u8_from_u32(uint8_t* buffer, uint32_t value) {
	for(int i = 0; i < 4; i++) {
        buffer[3 - i] = value & 0xFF;
        value >>= 8;
    }
}

uint16_t fletcher_checksum(uint8_t* data, size_t length) {
    uint8_t buffer[length];
    memcpy(buffer, data, length);

    uint8_t ck_a = 0, ck_b = 0;

    for(uint8_t i = 0; i < length; i++) {
        ck_a += buffer[i];
        ck_b += ck_a;
    }
    return (ck_a << 8) | ck_b;
}

void ade_data_to_frame_uart_send(Monitor_Data_t monitor_data, uint8_t* buffer) {
	for(int i = 0; i < 3; i++)
		buffer[2 - i] = (FRAME_UART_PREFIX & (0xFF << i*8)) >> i*8;
	u8_from_float(buffer + 3 + VRMS_OFFSET, monitor_data.data1.Vrms);
	u8_from_float(buffer + 3 + VPEAK_OFFSET, monitor_data.data1.Vpeak);
	u8_from_float(buffer + 3 + IRMS_OFFSET, monitor_data.data1.Irms);
	u8_from_float(buffer + 3 + IPEAK_OFFSET, monitor_data.data1.Ipeak);
	u8_from_float(buffer + 3 + POWER_OFFSET, monitor_data.data2.ActivePower);
	u8_from_float(buffer + 3 + REACTIVEPOWER_OFFSET, monitor_data.data2.ReactivePower);
	u8_from_float(buffer + 3 + APPARENTPOWER_OFFSET, monitor_data.data2.ApparantPower);
	u8_from_float(buffer + 3 + POWERFACTOR_OFFSET, monitor_data.data2.PowerFactor);
	u8_from_float(buffer + 3 + ENERGY_OFFSET, monitor_data.data3.ActiveEnergy);
	u8_from_u32(buffer + 3 + DAYS_OFFSET, monitor_data.data3.EnergyTimer.days);
	memcpy(buffer + 3 + HOUR_OFFSET, &monitor_data.data3.EnergyTimer.hours, 3);
//	memcpy(buffer + MINUTE_OFFSET, &monitor_data.data3.EnergyTimer.minutes);
//	memcpy(buffer + SECOND_OFFSET, &monitor_data.data3.EnergyTimer.seconds);
	u8_from_float(buffer + 3 + PKV_OFFSET, monitor_data.data4.User_PKV);
	u8_from_float(buffer + 3 + PKI_OFFSET, monitor_data.data4.User_PKI);
	u8_from_float(buffer + 3 + SAG_OFFSET, monitor_data.data4.User_SAG);
	memcpy(buffer + 3 + INTERVAL_OFFSET, &monitor_data.data4.User_Interval, 1);
	memcpy(buffer + 3 + RTC_OFFSET, &monitor_data.datetime, 7);

	uint16_t checksum = fletcher_checksum(buffer + 3, FRAME_MONITOR_BUFFER_SIZE - 5);

	buffer[FRAME_MONITOR_BUFFER_SIZE - 2] = checksum >> 8;
	buffer[FRAME_MONITOR_BUFFER_SIZE - 1] = checksum & 0xFF;
}
