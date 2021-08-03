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
#include "queue.h"

#include "stm32f407xx.h"
#include "ade7753.h"
#include "lcd5110.h"
#include "ade_value_scale.h"

uint32_t SystemCoreClock = 16000000;

float data0 = 0, data1 = 0, data2 = 0, data3 = 0, data4 = 0;

QueueHandle_t ade_queue_handle;

void lcd_handler(void* parameters);
void ade_handler(void* parameters);

int main() {

	xTaskCreate(lcd_handler, "LCD5110", 2048, NULL, 1, NULL);
	xTaskCreate(ade_handler, "ADE7753", 512, NULL, 1, NULL);

	ade_queue_handle = xQueueCreate(10, sizeof(ADE_Event_t));

	vTaskStartScheduler();

	while(1);
	return 0;
}

void lcd_handler(void* parameters) {
	LCD5110_Init(0x37);
	char buf[20];

	while(1) {
		LCD5110_Clear();

		sprintf(buf, "V: %.2f", (data0 * VRMS_SCALE_CONST) / 1000);
		LCD5110_Puts(buf, 1, 1);

		LCD5110_GotoXY(0, 10);
		sprintf(buf, "I: %.2f", data1 * IRMS_SCALE_CONST);
		LCD5110_Puts(buf, 1, 1);

		LCD5110_GotoXY(0, 20);
		sprintf(buf, "P: %.2f", data2 * POWER_SCALE_CONST / 1000);
		LCD5110_Puts(buf, 1, 1);

		LCD5110_GotoXY(0, 30);
		sprintf(buf, "Q: %.2f", data3 * REACTIVE_POWER_SCALE_CONST / 1000);
		LCD5110_Puts(buf, 1, 1);

//		LCD5110_GotoXY(0, 40);
//		sprintf(buf, "S: %d", data4);
//		LCD5110_Puts(buf, 1, 1);

		LCD5110_Refresh();

		vTaskDelay(200);
		taskYIELD();
	}
}

void ade_handler(void* parameters) {
	ADE_Event_t ade_event;

	ADE_Init();

//	CH1 full-scale 0.125V
//	PGA1 x16
//	PGA2 x2
	ADE_SetGain(FULL_SCALE_0125, GAIN_8, GAIN_2);

//	set POAM, CYCMODE
//	clear DISSAG
	ADE_WriteData(MODE,
			(0x000c
			& ~(1 << MODE_DISSAG))
			| (1 << MODE_CYCMODE)
			| (1 << MODE_POAM),
			2);

//	LINECYC = 200
	ADE_WriteData(LINECYC, 0x00c8, 2);

////	Sag Cycle: 3
//	ADE_WriteData(SAGCYC, 0x04, 1);
//
////	Sag level
//	ADE_WriteData(SAGLVL, 0x30, 1);
//
////	Vpeak level
//	ADE_WriteData(VPKLVL, 0xFF, 1);
//
////	Ipeak level
//	ADE_WriteData(IPKLVL, 0x10, 1);

//	set SAG, CYCEND, PKV, PKI
	ADE_WriteData(IRQEN,
			0x0040
			| (1 << IRQ_SAG)
			| (1 << IRQ_CYCEND)
			| (1 << IRQ_PKV)
			| (1 << IRQ_PKI),
			2);

//	clear STATUS Register
	ADE_ReadData(RSTSTATUS, 2);

	while(1) {
		if(ade_queue_handle != NULL)
			if(xQueueReceive(ade_queue_handle, &ade_event, portMAX_DELAY)) {
				switch (ade_event) {
					case ADE_INT_IRQ: {
						vTaskDelay(1);
						uint32_t rststatus = ADE_ReadData(RSTSTATUS, 2);

						if(rststatus & (1 << IRQ_SAG)) {

						}

						if(rststatus & (1 << IRQ_CYCEND)) {
							data2 = ADE_ReadData(LAENERGY, 3);
							data3 = ade_signed_value(ADE_ReadData(LVARENERGY, 3), 23);
						}

						if(rststatus & (1 << IRQ_PKV)) {

						}

						if(rststatus & (1 << IRQ_PKI)) {

						}

						break;
					}

					case ADE_INT_ZX:
						data0 = ADE_ReadData(VRMS, 3);
						data1 = ADE_ReadData(IRMS, 3);
						break;

					case ADE_INT_SAG:

						break;

					default:
						break;
				}
			}

		taskYIELD();
	}
}

void EXTI15_10_IRQHandler(void) {
	static uint8_t zx_count = 0;
	ADE_Event_t ade_int;
    uint32_t pending = EXTI->PR;

    if(pending & (1 << PIN_ZX_IT)) {
        EXTI->PR |= 1 << PIN_ZX_IT;		// clear pending flag, otherwise we'd get endless interrupts
        // handle pin ZX here
		if(++zx_count >= 50) {
			zx_count = 0;
			ade_int = ADE_INT_ZX;
			xQueueSendToFrontFromISR(ade_queue_handle, &ade_int, NULL);
		}
    }

    if(pending & (1 << PIN_IRQ_IT)) {
        EXTI->PR |= 1 << PIN_IRQ_IT;	// clear pending flag, otherwise we'd get endless interrupts
        // handle pin IRQ here
        ade_int = ADE_INT_IRQ;
		xQueueSendFromISR(ade_queue_handle, &ade_int, NULL);
    }
}
