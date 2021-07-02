#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "ade7753.h"
#include "ds1307.h"
#include "keypad.h"
#include "lcd5110.h"
#include "lcd_control.h"

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

uint32_t SystemCoreClock = 16000000;

LCD_Data_Screen1_t screen1_ade_data;
LCD_Data_Screen2_t screen2_ade_data;
LCD_Data_Screen3_t screen3_ade_data;
LCD_Data_Screen4_t screen4_ade_data;

QueueHandle_t keypad_queue_handle;
QueueHandle_t keypad_hold_queue_handle;
//QueueHandle_t ade_queue_handle;

void keypad_handler(void* parameters);
void lcd_handler(void* parameters);
void ade_handler(void* parameters);
void rtc_handler(void* parameters);
void usart_handler(void* parameters);

void LED_Init();

int main(void) {
	LED_Init();

	TaskHandle_t keypad_handle;
	TaskHandle_t lcd_handle;
	TaskHandle_t ade_handle;
	TaskHandle_t rtc_handle;
	TaskHandle_t usart_handle;

	xTaskCreate(keypad_handler, "Keypad", 256, NULL, 2, &keypad_handle);
	xTaskCreate(lcd_handler, "LCD5110", 1024, NULL, 2, &lcd_handle);
	xTaskCreate(ade_handler, "ADE7753", 256, NULL, 2, &ade_handle);
	xTaskCreate(rtc_handler, "DS1307", 256, NULL, 2, &rtc_handle);
	xTaskCreate(usart_handler, "USART", 256, NULL, 2, &usart_handle);

	keypad_queue_handle = xQueueCreate(10, sizeof(KEYPAD_Button_t));
	keypad_hold_queue_handle = xQueueCreate(10, sizeof(KEYPAD_Button_t));
//	ade_queue_handle = xQueueCreate(10, sizeof(uint8_t));

	vTaskStartScheduler();

	while(1);
	return 0;
}

void keypad_handler(void* parameters) {
	KEYPAD_Init(KEYPAD_Type_Large);
	KEYPAD_Button_t Keypad_Button, Keypad_prev = KEYPAD_NOPRESSED;
	uint16_t keypad_hold_timeout = 0;

	while(1) {
		KEYPAD_Update();
		Keypad_Button = KEYPAD_Read();

		if(Keypad_Button != KEYPAD_NOPRESSED) {
			if(Keypad_prev == KEYPAD_NOPRESSED)
				xQueueSend(keypad_queue_handle, (void*) &Keypad_Button, (TickType_t) 0);
			else if(Keypad_Button == Keypad_prev && ++keypad_hold_timeout > 3000) {
				keypad_hold_timeout = 0;
				xQueueSend(keypad_hold_queue_handle, (void*) &Keypad_Button, (TickType_t) 0);
			}
		}

		Keypad_prev = Keypad_Button;
		vTaskDelay(1);
		taskYIELD();
	}
}

void lcd_handler(void* parameters) {
	LCD5110_Init(0x37);

	memset(&screen1_ade_data, 0, sizeof(screen1_ade_data));
	memset(&screen2_ade_data, 0, sizeof(screen2_ade_data));
	memset(&screen3_ade_data, 0, sizeof(screen3_ade_data));
	memset(&screen4_ade_data, 0, sizeof(screen4_ade_data));

	lcd_screen_1_reset();
	lcd_screen_2_reset();
	lcd_screen_3_reset();
	lcd_screen_4_reset();

	KEYPAD_Button_t keypad;
	LCD_Screen_t screen = LCD_Screen_3;

	while(1) {
		if(keypad_queue_handle != NULL) {
			if(xQueueReceive(keypad_queue_handle, &keypad, (TickType_t) 10) == pdPASS) {
				switch (keypad) {
					case KEYPAD_Button_B:
						screen = LCD_Screen_2;
						break;
					case KEYPAD_Button_C:
						screen = LCD_Screen_3;
						break;
					case KEYPAD_Button_D:
						screen = LCD_Screen_4;
						break;
					case KEYPAD_Button_A:
					default:
						screen = LCD_Screen_1;
						break;
				}
			}
		}

		if(keypad_hold_queue_handle != NULL) {
			if(xQueueReceive(keypad_hold_queue_handle, &keypad, (TickType_t) 10) == pdPASS) {
				switch (keypad) {
					case KEYPAD_Button_D:
						lcd_screen_4_switch_mode();
						break;
					default:
						break;
				}
			}
		}

		switch (screen) {
			case LCD_Screen_2:
				lcd_screen_2_refresh();
				break;
			case LCD_Screen_3:
				lcd_screen_3_refresh();
				break;
			case LCD_Screen_4:
				lcd_screen_4_refresh();
				break;
			case LCD_Screen_1:
			default:
				lcd_screen_1_refresh();
				break;
		}

		vTaskDelay(500);
		taskYIELD();
	}
}

void ade_handler(void* parameters) {
//	char buffer[20];
	ADE_Init();
	ADE_WriteData(IRQEN, 0x0010, 2);
	ADE_ReadData(RSTSTATUS, 2);

	while(1) {
//		ADE_ReadData(RSTSTATUS, 2);
		taskYIELD();
	}
}

void rtc_handler(void* parameters) {
	ds1307_init();

	RTC_time_t rtc_time, rtc_time_prev;

	LCD_Screen4_RTC_t screen4_rtc;

	do {
		ds1307_get_current_time(&rtc_time_prev);
		vTaskDelay(10);
		taskYIELD();
	} while(!rtc_time_prev.time_validity);

	while(1) {
		ds1307_get_current_time(&rtc_time);
		ds1307_get_current_date(&screen4_rtc.date);

		if(rtc_time.time_validity) {
			uint32_t time_in_sec, time_in_sec_prev;
			time_in_sec = rtc_time.hours*3600 + rtc_time.minutes*60 + rtc_time.seconds;
			time_in_sec_prev = rtc_time_prev.hours*3600 + rtc_time_prev.minutes*60 + rtc_time_prev.seconds;
			int time_gap = time_in_sec - time_in_sec_prev;

			if(time_gap < 0) time_gap += 24*3600;
			if(time_gap == 1) lcd_screen_3_timer_count_up();

			rtc_time_prev = rtc_time;

			screen4_rtc.time = rtc_time;
			lcd_screen_4_rtc_update(screen4_rtc);
		}

		vTaskDelay(200);
		taskYIELD();
	}
}

void usart_handler(void* parameters) {



	while(1) {
		taskYIELD();
	}
}

void LED_Init() {
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

//	R_LED_ON;
//	G_LED_ON;
//	B_LED_ON;
}

void EXTI15_10_IRQHandler(void) {
    uint32_t pending = EXTI->PR;

    if(pending & (1 << PIN_SAG)) {
        EXTI->PR |= 1 << PIN_SAG;		// clear pending flag, otherwise we'd get endless interrupts
        // handle pin SAG here
    }

    if(pending & (1 << PIN_ZX_IT)) {
        EXTI->PR |= 1 << PIN_ZX_IT;		// clear pending flag, otherwise we'd get endless interrupts
        // handle pin ZX here
    }

    if(pending & (1 << PIN_IRQ_IT)) {
        EXTI->PR |= 1 << PIN_IRQ_IT;	// clear pending flag, otherwise we'd get endless interrupts
        // handle pin I here
        vTaskDelay(5);
        uint32_t rststatus = ADE_ReadData(RSTSTATUS, 2);

        if(rststatus & (1 << IRQ_CYCEND)) {
//        	uint32_t laenergy = ADE_ReadData(LAENERGY, 3);
//        	uint32_t lvaenergy = ADE_ReadData(LVAENERGY, 3);
//        	uint32_t lvarenergy = ADE_ReadData(LVARENERGY, 3);
        }

        if(rststatus & (1 << IRQ_PKV)) {

        }

        if(rststatus & (1 << IRQ_PKI)) {

		}
    }
}
