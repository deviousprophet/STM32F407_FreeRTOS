#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"

#include "ade7753.h"
#include "ds1307.h"
#include "keypad.h"
#include "lcd5110.h"
#include "lcd_control.h"

//LED Pin
#define PORT_LED		GPIOE
#define PIN_IPKL_LED	GPIO_PIN_NO_1
#define PIN_VPKL_LED	GPIO_PIN_NO_3
#define PIN_SAG_LED		GPIO_PIN_NO_5

#define SAG_LED_ON		GPIO_WriteToOutputPin(PORT_LED, PIN_SAG_LED, 0)
#define VPKL_LED_ON		GPIO_WriteToOutputPin(PORT_LED, PIN_VPKL_LED, 0)
#define IPKL_LED_ON		GPIO_WriteToOutputPin(PORT_LED, PIN_IPKL_LED, 0)

#define SAG_LED_OFF		GPIO_WriteToOutputPin(PORT_LED, PIN_SAG_LED, 1)
#define VPKL_LED_OFF	GPIO_WriteToOutputPin(PORT_LED, PIN_VPKL_LED, 1)
#define IPKL_LED_OFF	GPIO_WriteToOutputPin(PORT_LED, PIN_IPKL_LED, 1)

uint32_t SystemCoreClock = 16000000;

static void keypad_handler(void* parameters);
static void lcd_handler(void* parameters);
static void ade_handler(void* parameters);
static void rtc_handler(void* parameters);

void LED_Init();

int main(void) {
	LED_Init();

	TaskHandle_t keypad_handle;
	TaskHandle_t lcd_handle;
	TaskHandle_t ade_handle;
	TaskHandle_t rtc_handle;

	xTaskCreate(keypad_handler, "Keypad", 200, NULL, 2, &keypad_handle);
	xTaskCreate(lcd_handler, "LCD5110", 200, NULL, 2, &lcd_handle);
	xTaskCreate(ade_handler, "ADE7753", 200, NULL, 2, &ade_handle);
	xTaskCreate(rtc_handler, "DS1307", 200, NULL, 2, &rtc_handle);
	vTaskStartScheduler();

	while(1);
	return 0;
}

static void keypad_handler(void* parameters) {
	KEYPAD_Init(KEYPAD_Type_Large);
	while(1) {
		KEYPAD_Update();
		taskYIELD();
	}
}

static void lcd_handler(void* parameters) {
	LCD5110_Init(0x37);
	KEYPAD_Button_t Keypad_Button, Keypad_prev = KEYPAD_NOPRESSED;
	char buffer[20];

	while(1) {
		Keypad_Button = KEYPAD_Read();
		if(Keypad_Button != KEYPAD_NOPRESSED && Keypad_prev == KEYPAD_NOPRESSED) {
			LCD5110_Clear();
			sprintf(buffer, "%u", Keypad_Button);
			LCD5110_Puts(buffer, LCD5110_Pixel_Set, LCD5110_FontSize_5x7);
			LCD5110_Refresh();
		}
		Keypad_prev = Keypad_Button;
		vTaskDelay(10);
		taskYIELD();
	}
}

static void ade_handler(void* parameters) {
	ADE_Init();

	while(1) {
		taskYIELD();
	}
}

static void rtc_handler(void* parameters) {
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

//	SAG
	GpioLed.GPIO_PinConfig.GPIO_PinNumber = PIN_SAG_LED;
	GPIO_Init(&GpioLed);

//	VPKL
	GpioLed.GPIO_PinConfig.GPIO_PinNumber = PIN_VPKL_LED;
	GPIO_Init(&GpioLed);

//	IPKL
	GpioLed.GPIO_PinConfig.GPIO_PinNumber = PIN_IPKL_LED;
	GPIO_Init(&GpioLed);

	SAG_LED_OFF;
	VPKL_LED_OFF;
	IPKL_LED_OFF;
}

void EXTI15_10_IRQHandler(void) {
//    uint32_t pending = EXTI->PR;
//    if(pending & (1 << PIN_SAG)) {
//        EXTI->PR |= 1 << PIN_SAG;		// clear pending flag, otherwise we'd get endless interrupts
//        // handle pin SAG here
//
//    }
//
//    if(pending & (1 << PIN_ZX_IT)) {
//        EXTI->PR |= 1 << PIN_ZX_IT;		// clear pending flag, otherwise we'd get endless interrupts
//        // handle pin ZX here
//    }
//
//    if(pending & (1 << PIN_IRQ_IT)) {
//        EXTI->PR |= 1 << PIN_IRQ_IT;	// clear pending flag, otherwise we'd get endless interrupts
//        // handle pin I here
//        for(int i = 0; i < 5000; i++);
//        ADE_ReadData(RSTSTATUS, 2);
//    }
}
