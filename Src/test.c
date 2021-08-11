#include "main.h"

#ifdef USE_TEST_C

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "stm32f407xx.h"
#include "stm32f407xx_gpio.h"
#include "stm32f407xx_usart.h"
#include "ade7753.h"
#include "ds1307.h"
#include "keypad.h"
#include "lcd5110.h"
#include "lcd_control.h"
#include "ade_value_scale.h"

#define INTERVAL_SECOND	1

//LED Pin
#define PORT_LED		GPIOE
#define PIN_R_LED		GPIO_PIN_NO_1
#define PIN_G_LED		GPIO_PIN_NO_3
#define PIN_B_LED		GPIO_PIN_NO_5

#define R_LED_ON		GPIO_WriteToOutputPin(PORT_LED, PIN_R_LED, 0)
#define G_LED_ON		GPIO_WriteToOutputPin(PORT_LED, PIN_G_LED, 0)
#define B_LED_ON		GPIO_WriteToOutputPin(PORT_LED, PIN_B_LED, 0)

#define R_LED_OFF		GPIO_WriteToOutputPin(PORT_LED, PIN_R_LED, 1)
#define G_LED_OFF		GPIO_WriteToOutputPin(PORT_LED, PIN_G_LED, 1)
#define B_LED_OFF		GPIO_WriteToOutputPin(PORT_LED, PIN_B_LED, 1)

#define KEYPAD_HOLD_TIMEOUT		2000

uint32_t SystemCoreClock = 16000000;

float irms = 0, pki = 0;
uint32_t status = 0;

QueueHandle_t ade_queue_handle;

void lcd_handler(void* parameters);
void ade_handler(void* parameters);

void led_init() {
//	LED Init
	GPIO_Handle_t GpioLed;
	GpioLed.pGPIOx = PORT_LED;
	GpioLed.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	GpioLed.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
	GpioLed.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
	GpioLed.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;

//	Red LED
	GpioLed.GPIO_PinConfig.GPIO_PinNumber = PIN_R_LED;
	GPIO_Init(&GpioLed);

//	Green LED
	GpioLed.GPIO_PinConfig.GPIO_PinNumber = PIN_G_LED;
	GPIO_Init(&GpioLed);

//	Blue LED
	GpioLed.GPIO_PinConfig.GPIO_PinNumber = PIN_B_LED;
	GPIO_Init(&GpioLed);

	R_LED_OFF;
	G_LED_OFF;
	B_LED_OFF;
}

int main(void) {
	led_init();

	xTaskCreate(lcd_handler, "LCD5110", 2048, NULL, 1, NULL);
	xTaskCreate(ade_handler, "ADE7753", 512, NULL, 1, NULL);

	ade_queue_handle = xQueueCreate(20, sizeof(ADE_Event_t));

	vTaskStartScheduler();

	while(1);
	return 0;
}

void lcd_handler(void* parameters) {
	LCD5110_Init(0x37);
	char buf[20];

	while(1) {
		LCD5110_Clear();

		sprintf(buf, "IRMS %.2f", irms / 1000);
		LCD5110_Puts(buf, 1, 1);

		sprintf(buf, "PKI %.2f", pki / 1000);
		LCD5110_GotoXY(0, 10);
		LCD5110_Puts(buf, 1, 1);

		sprintf(buf, "STATUS %lx", status);
		LCD5110_GotoXY(0, 20);
		LCD5110_Puts(buf, 1, 1);

		LCD5110_Refresh();
		vTaskDelay(200);
		taskYIELD();
	}
}

void ade_handler(void* parameters) {
	ADE_Event_t ade_event;
//	uint32_t status;

	ADE_Init();
	ADE_SetGain(FULL_SCALE_0125, GAIN_8, GAIN_2);

	ADE_WriteData(IPKLVL, 0x10 >> 1, 1);

/* set ZXTO */
	ADE_WriteData(IRQEN,
			0x0040
			| (1 << IRQ_ZXTO),
			2);

/* clear STATUS Register */
	ADE_ReadData(RSTSTATUS, 2);

	while(1) {
		if(ade_queue_handle != NULL) {
			if(xQueueReceive(ade_queue_handle, &ade_event, portMAX_DELAY)) {
				switch (ade_event) {
					case ADE_INT_IRQ:
						G_LED_OFF;
						ADE_ReadData(RSTSTATUS, 2);
						break;

					case ADE_INT_ZX:
						pki = rescale_hex_to_user_pki(ADE_ReadData(IPKLVL, 1)) * 2;
						irms = ade_scale_irms(ADE_ReadData(IRMS, 3));

						status = ADE_ReadData(RSTSTATUS, 2);

						if(status & (1 << IRQ_PKI))
							R_LED_ON;
						else R_LED_OFF;

						G_LED_ON;
						break;
					default:
						break;
				}
			}
		}
		taskYIELD();
	}
}

void EXTI15_10_IRQHandler(void) {
	ADE_Event_t ade_int;
	static uint8_t zx_count = 0;
    uint32_t pending = EXTI->PR;

    if(pending & (1 << PIN_ZX_IT)) {
        EXTI->PR |= 1 << PIN_ZX_IT;		// clear pending flag, otherwise we'd get endless interrupts
        // handle pin ZX here
        if(++zx_count >= 25) {
			ade_int = ADE_INT_ZX;
			xQueueSendToFrontFromISR(ade_queue_handle, &ade_int, NULL);
			zx_count = 0;
        }
    }

    if(pending & (1 << PIN_IRQ_IT)) {
        EXTI->PR |= 1 << PIN_IRQ_IT;	// clear pending flag, otherwise we'd get endless interrupts
        // handle pin IRQ here
        ade_int = ADE_INT_IRQ;
		xQueueSendFromISR(ade_queue_handle, &ade_int, NULL);
    }
}

#endif
