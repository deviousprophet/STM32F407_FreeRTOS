/*
 * test.c
 *
 *  Created on: Jul 15, 2021
 *      Author: deviousprophet
 */


#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include "FreeRTOS.h"
#include "task.h"

#include "stm32f407xx.h"
#include "ade7753.h"
#include "lcd5110.h"
#include "ade_value_scale.h"

uint32_t SystemCoreClock = 16000000;

int data1 = 0, data2 = 0, data3 = 0;
uint32_t data4 = 0;

void lcd_handler(void* parameters);
void ade_handler(void* parameters);

int main() {

	xTaskCreate(lcd_handler, "LCD5110", 2048, NULL, 1, NULL);
	xTaskCreate(ade_handler, "ADE7753", 512, NULL, 1, NULL);

	vTaskStartScheduler();

	while(1);
	return 0;
}

void lcd_handler(void* parameters) {
	LCD5110_Init(0x37);
	char buf[20];
	while(1) {
		LCD5110_Clear();
		sprintf(buf, "P: %d", data1);
		LCD5110_Puts(buf, 1, 1);

		LCD5110_GotoXY(0, 10);
		sprintf(buf, "Q: %d", data2);
		LCD5110_Puts(buf, 1, 1);

		LCD5110_GotoXY(0, 20);
		sprintf(buf, "S: %d", data3);
		LCD5110_Puts(buf, 1, 1);

		LCD5110_GotoXY(0, 30);
		sprintf(buf, "VRMS: %x", data4);
		LCD5110_Puts(buf, 1, 1);

		LCD5110_Refresh();

		vTaskDelay(200);
		taskYIELD();
	}
}

void ade_handler(void* parameters) {
	ADE_Init();

	ADE_WriteData(GAIN, 0x34, 1);
	ADE_WriteData(MODE, 0x008c, 2);
	ADE_WriteData(LINECYC, 0x012c, 2);
	ADE_WriteData(IRQEN, 0x0044, 2);
	ADE_ReadData(RSTSTATUS, 2);

	while(1) {
		data4 = ADE_ReadData(VRMS, 2);
		vTaskDelay(200);
		taskYIELD();
	}
}

void EXTI15_10_IRQHandler(void) {
    uint32_t pending = EXTI->PR;

    if(pending & (1 << PIN_IRQ_IT)) {
        EXTI->PR |= 1 << PIN_IRQ_IT;		// clear pending flag, otherwise we'd get endless interrupts

//        data2 = ADE_ReadData(RSTSTATUS, 2);
		data1 = ade_signed_value(ADE_ReadData(LAENERGY, 3), 23);
		data2 = ade_signed_value(ADE_ReadData(LVARENERGY, 3), 23);
		data3 = ADE_ReadData(LVAENERGY, 3);

		ADE_ReadData(RSTSTATUS, 2);
		ADE_ReadData(RSTSTATUS, 2);
    }
}
