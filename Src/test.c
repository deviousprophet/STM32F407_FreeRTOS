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

int data0 = 0, data1 = 0, data2 = 0, data3 = 0, data4 = 0;

QueueHandle_t ade_queue_handle;

void lcd_handler(void* parameters);
void ade_handler(void* parameters);

int main() {

	xTaskCreate(lcd_handler, "LCD5110", 2048, NULL, 1, NULL);
	xTaskCreate(ade_handler, "ADE7753", 512, NULL, 1, NULL);

	ade_queue_handle = xQueueCreate(10, sizeof(ADE_INT_t));

	vTaskStartScheduler();

	while(1);
	return 0;
}

void lcd_handler(void* parameters) {
	LCD5110_Init(0x37);
	char buf[20];

	while(1) {
		LCD5110_Clear();

		sprintf(buf, "VRMS: %d", data0);
		LCD5110_Puts(buf, 1, 1);

		LCD5110_GotoXY(0, 10);
		sprintf(buf, "IRMS: %d", data1);
		LCD5110_Puts(buf, 1, 1);

		LCD5110_GotoXY(0, 20);
		sprintf(buf, "P: %d", data2);
		LCD5110_Puts(buf, 1, 1);

		LCD5110_GotoXY(0, 30);
		sprintf(buf, "Q: %d", data3);
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
	ADE_INT_t ade_int;
	uint32_t rststatus;

	ADE_Init();

//	CH1 full-scale 0.125V
//	PGA1 x16
//	PGA2 x2
	ADE_WriteData(GAIN, 0x34, 1);

//	set POAM, CYCMODE
//	clear DISSAG
	ADE_WriteData(MODE,
			(0x000c
			& ~(1 << MODE_DISSAG))
			| (1 << MODE_CYCMODE)
			| (1 << MODE_POAM),
			2);

//	LINECYC = 200
	ADE_WriteData(LINECYC, 200, 2);

////	Sag Cycle: 3
//	ADE_WriteData(SAGCYC, 0x04, 1);
//
////	Sag level ~ 110V
//	ADE_WriteData(SAGLVL, 0x00, 1);

//	set SAG, CYCEND, PKV, PKI
	ADE_WriteData(IRQEN,
			0x0040
//			| (1 << IRQ_SAG)
			| (1 << IRQ_CYCEND),
//			| (1 << IRQ_PKV)
//			| (1 << IRQ_PKI),
			2);

//	clear STATUS Register
	ADE_ReadData(RSTSTATUS, 2);

	while(1) {
		if(ade_queue_handle != NULL)
			if(xQueueReceive(ade_queue_handle, &ade_int, portMAX_DELAY) == pdPASS) {
				switch (ade_int) {
					case ADE_INT_ZX:
						data0 = ADE_ReadData(VRMS, 3);
//						data1 = ADE_ReadData(IRMS, 3);
						ADE_ReadData(RSTSTATUS, 2);
						break;

					case ADE_INT_IRQ:
						vTaskDelay(1);
						rststatus = ADE_ReadData(RSTSTATUS, 2);

						if(rststatus & (1 << IRQ_CYCEND)) {
							data2 = ade_signed_value(ADE_ReadData(LAENERGY, 3), 23);
//							data3 = ade_signed_value(ADE_ReadData(LVARENERGY, 3), 23);
//							data4 = ADE_ReadData(LVAENERGY, 3);
						}

						if(rststatus & (1 << IRQ_PKV)) {

						}

						if(rststatus & (1 << IRQ_PKI)) {

						}

//						ADE_ReadData(RSTSTATUS, 2);
//						ADE_ReadData(RSTSTATUS, 2);
//						ADE_ReadData(RSTSTATUS, 2);
//						ADE_ReadData(RSTSTATUS, 2);
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
	ADE_INT_t ade_int;
    uint32_t pending = EXTI->PR;

//    if(pending & (1 << PIN_SAG)) {
//        EXTI->PR |= 1 << PIN_SAG;		// clear pending flag, otherwise we'd get endless interrupts
//        // handle pin SAG here
//		ade_int = ADE_INT_SAG;
//		xQueueSendFromISR(ade_queue_handle, &ade_int, NULL);
//    }

    if(pending & (1 << PIN_ZX_IT)) {
        EXTI->PR |= 1 << PIN_ZX_IT;		// clear pending flag, otherwise we'd get endless interrupts
        // handle pin ZX here
		if(++zx_count >= 25) {
			zx_count = 0;
			ade_int = ADE_INT_ZX;
			xQueueSendFromISR(ade_queue_handle, &ade_int, NULL);
		}
    }

    if(pending & (1 << PIN_IRQ_IT)) {
        EXTI->PR |= 1 << PIN_IRQ_IT;	// clear pending flag, otherwise we'd get endless interrupts
        // handle pin IRQ here
        ade_int = ADE_INT_IRQ;
		xQueueSendFromISR(ade_queue_handle, &ade_int, NULL);
    }
}
