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
#include "ds1307.h"
#include "keypad.h"
#include "lcd5110.h"
#include "lcd_control.h"
#include "ade_value_scale.h"

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

typedef struct {
	uint8_t address;
	uint32_t data;
	uint8_t bytes;
} ADE_Write_Data_t;

LCD_Data_Screen1_t screen1_data;
LCD_Data_Screen2_t screen2_data;
LCD_Data_Screen3_t screen3_data;
LCD_Data_Screen4_t screen4_data;

QueueHandle_t keypad_queue_handle;
QueueHandle_t rtc_queue_handle;
QueueHandle_t ade_write_queue_handle;

void lcd_handler(void* parameters);
void ade_handler(void* parameters);
void keypad_handler(void* parameters);
void rtc_handler(void* parameters);
void usart_handler(void* parameters);

void LED_Init();

void SEGGER_setup() {
	RCC->CFGR = 0
			| (1 << 10)
			| (1 << 12)
			| (1 << 15);
	(*(volatile uint32_t*) 0XE0001000) |= (1 << 0);
	SEGGER_UART_init(500000);
	SEGGER_SYSVIEW_Conf();
}

int main(void) {
//	SEGGER_setup();

	LED_Init();

	xTaskCreate(lcd_handler, "LCD5110", 2048, NULL, 1, NULL);
	xTaskCreate(ade_handler, "ADE7753", 512, NULL, 1, NULL);
	xTaskCreate(keypad_handler, "Keypad", 256, NULL, 1, NULL);
	xTaskCreate(rtc_handler, "DS1307", 512, NULL, 1, NULL);
	xTaskCreate(usart_handler, "USART", 512, NULL, 1, NULL);

	keypad_queue_handle = xQueueCreate(5, sizeof(KEYPAD_Button_t));
	rtc_queue_handle = xQueueCreate(1, sizeof(DS1307_DateTime_t));
	ade_write_queue_handle = xQueueCreate(10, sizeof(ADE_Write_Data_t));

	vTaskStartScheduler();

	while(1);
	return 0;
}

void lcd_handler(void* parameters) {
	LCD5110_Init(0x37);

	memset(&screen1_data, 0, sizeof(screen1_data));
	memset(&screen2_data, 0, sizeof(screen2_data));
	memset(&screen3_data, 0, sizeof(screen3_data));
	memset(&screen4_data, 0, sizeof(screen4_data));

	lcd_screen_1_clear();
	lcd_screen_2_clear();
	lcd_screen_3_clear();
	lcd_screen_4_clear();

	KEYPAD_Button_t keypad;
	LCD_Screen_t screen = LCD_Screen_4;

//	ADE_Write_Data_t ade_write_data;

	while(1) {
		if(keypad_queue_handle != NULL) {
			if(xQueueReceive(keypad_queue_handle, &keypad, (TickType_t) 10) == pdPASS) {
				switch (keypad) {
					case KEYPAD_Button_0:
					case KEYPAD_Button_1:
					case KEYPAD_Button_2:
					case KEYPAD_Button_3:
					case KEYPAD_Button_4:
					case KEYPAD_Button_5:
					case KEYPAD_Button_6:
					case KEYPAD_Button_7:
					case KEYPAD_Button_8:
					case KEYPAD_Button_9:
						if(lcd_screen_4_mode() == S4_CONFIG_PARAMS_DISPLAY)
							lcd_enter_params_value(keypad);
						if(lcd_screen_4_mode() == S4_CONFIG_DATETIME_DISPLAY)
							lcd_enter_datetime_value(keypad);
						break;
					case KEYPAD_Button_A:
						if(!lcd_is_busy()) screen = LCD_Screen_1;
						break;
					case KEYPAD_Button_B:
						if(!lcd_is_busy()) screen = LCD_Screen_2;
						break;
					case KEYPAD_Button_C:
						if(!lcd_is_busy()) screen = LCD_Screen_3;
						break;
					case KEYPAD_Button_D:
						if(!lcd_is_busy()) screen = LCD_Screen_4;
						else {
							switch (lcd_screen_4_mode()) {
								case S4_CONFIG_DISPLAY:
									lcd_screen_4_next_config_option();
									break;
								case S4_CONFIG_PARAMS_DISPLAY:
									if(lcd_screen_4_config_item() % 2)
										lcd_enter_params_value(keypad);
									else lcd_screen_4_next_config_item();
									break;
								case S4_CONFIG_DATETIME_DISPLAY:
									lcd_screen_4_next_config_item();
									break;
								default:
									break;
							}
						}
						break;

					case KEYPAD_Button_HASH:
						if(lcd_is_busy()) {
							if(screen == LCD_Screen_3)
								lcd_screen_3_clear();
							if(screen == LCD_Screen_4) {
								switch (lcd_screen_4_mode()) {
									case S4_CONFIG_DISPLAY:
										if(lcd_screen_4_config_option() == Config_Params)
											lcd_screen_4_switch_mode(S4_CONFIG_PARAMS_DISPLAY);
										else lcd_screen_4_switch_mode(S4_CONFIG_DATETIME_DISPLAY);
										break;
									case S4_CONFIG_PARAMS_DISPLAY:
									case S4_CONFIG_DATETIME_DISPLAY:
										if(lcd_screen_4_config_item() == Config_interval_selected)
											lcd_screen_4_next_interval_set();
										else lcd_screen_4_config_select(CONFIG_SELECT);
										break;
									case S4_COMMIT_DISPLAY:
										if(lcd_screen_4_config_option() == Config_Params) {
											screen4_data = lcd_screen_4_commit_parameters();
										} else {
											DS1307_DateTime_t config_rtc = lcd_screen_4_commit_rtc();
											xQueueSend(rtc_queue_handle, (void*) &config_rtc, (TickType_t) 0);
										}
										lcd_screen_4_switch_mode(S4_CONFIG_DISPLAY);
										break;
									default:
										break;
								}
							}
						}
						break;
					case KEYPAD_Button_STAR:
						if(lcd_is_busy()) {
							if(screen == LCD_Screen_3)
								lcd_screen_3_switch_mode(S3_NORMAL_DISPLAY);
							if(screen == LCD_Screen_4) {
								switch (lcd_screen_4_mode()) {
									case S4_CONFIG_PARAMS_DISPLAY:
									case S4_CONFIG_DATETIME_DISPLAY:
										if(lcd_screen_4_config_item() % 2)
											lcd_screen_4_config_select(CONFIG_DESELECT);
										else lcd_screen_4_switch_mode(S4_COMMIT_DISPLAY);
										break;
									case S4_COMMIT_DISPLAY:
										lcd_screen_4_switch_mode(S4_CONFIG_DISPLAY);
										break;
									default:
										break;
								}
							}
						}
						break;

					case KEYPAD_Button_HOLD_0:
					case KEYPAD_Button_HOLD_1:
					case KEYPAD_Button_HOLD_2:
					case KEYPAD_Button_HOLD_3:
					case KEYPAD_Button_HOLD_4:
					case KEYPAD_Button_HOLD_5:
					case KEYPAD_Button_HOLD_6:
					case KEYPAD_Button_HOLD_7:
					case KEYPAD_Button_HOLD_8:
					case KEYPAD_Button_HOLD_9:
						break;

					case KEYPAD_Button_HOLD_A:
						break;
					case KEYPAD_Button_HOLD_B:
						break;
					case KEYPAD_Button_HOLD_C:
						if(!lcd_is_busy() && screen == LCD_Screen_3)
							lcd_screen_3_switch_mode(S3_RESET_CONFIRM_DISPLAY);
						break;
					case KEYPAD_Button_HOLD_D:
						if(!lcd_is_busy()) {
							if(screen == LCD_Screen_4)
								lcd_screen_4_switch_mode(S4_CONFIG_DISPLAY);
						} else if(lcd_screen_4_mode() == S4_CONFIG_DISPLAY)
							lcd_screen_4_switch_mode(S4_NORMAL_DISPLAY);
						break;


					case KEYPAD_Button_HOLD_HASH:
					case KEYPAD_Button_HOLD_STAR:
						break;
					default:
						break;
				}
			}
		}

		lcd_screen_refresh(screen);
		vTaskDelay(200);
		taskYIELD();
	}
}

void ade_handler(void* parameters) {
	ADE_Write_Data_t write_data;

	ADE_Init();

	ADE_WriteData(GAIN, 0x34, 1);
	ADE_WriteData(MODE, 0x008c, 2);
	ADE_WriteData(LINECYC, 0x00c8, 2);
	ADE_WriteData(IRQEN, 0x0044, 2);
	ADE_ReadData(RSTSTATUS, 2);

	while(1) {
		if(ade_write_queue_handle != NULL)
			if(xQueueReceive(ade_write_queue_handle, &write_data, (TickType_t) 10) == pdPASS)
				ADE_WriteData(write_data.address, write_data.data, write_data.bytes);

		vTaskDelay(100);
		taskYIELD();
	}
}

void keypad_handler(void* parameters) {
	KEYPAD_Init(KEYPAD_Type_Large);
	KEYPAD_Button_t Keypad_Hold_Button, Keypad_Button, Keypad_prev = KEYPAD_NOPRESSED;
	int hold_timeout = 400;

	while(1) {
		KEYPAD_Update();
		Keypad_Button = KEYPAD_Read();

		if(Keypad_Button != KEYPAD_NOPRESSED && Keypad_Button == Keypad_prev) {
			if(hold_timeout > 0) hold_timeout--;
			else if (!hold_timeout) {
				hold_timeout--;
				Keypad_Hold_Button = KEYPAD_Hold_Button(Keypad_Button);
				xQueueSend(keypad_queue_handle, (void*) &Keypad_Hold_Button, (TickType_t) 0);
			}
		}
		if(Keypad_Button == KEYPAD_NOPRESSED && Keypad_prev != KEYPAD_NOPRESSED)
			if(hold_timeout > 0)
				xQueueSend(keypad_queue_handle, (void*) &Keypad_prev, (TickType_t) 0);
		if(Keypad_Button == KEYPAD_NOPRESSED && Keypad_prev != KEYPAD_NOPRESSED)
			hold_timeout = 600;

		Keypad_prev = Keypad_Button;
		vTaskDelay(5);
		taskYIELD();
	}
}

void rtc_handler(void* parameters) {
	DS1307_DateTime_t rtc_datetime, rtc_config_data;
	int sec, sec_prev = 0;

	while(DS1307_Init() != DS1307_Result_OK) vTaskDelay(10);

	while(1) {
		if(rtc_queue_handle != NULL) {
			if(xQueueReceive(rtc_queue_handle, &rtc_config_data, (TickType_t) 10) == pdPASS)
				DS1307_SetDateTime(&rtc_config_data);
		}

		DS1307_GetDateTime(&rtc_datetime);
		sec = rtc_datetime.seconds;
		if((sec - sec_prev == 1) || (sec_prev - sec == 59)) lcd_screen_3_timer_count_up();
		lcd_screen_4_rtc_update(rtc_datetime);

		sec_prev = sec;
		vTaskDelay(100);
		taskYIELD();
	}
}

void usart_handler(void* parameters) {

	while(1) {
		vTaskDelay(100);
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
        screen1_data.Vrms = ade_scale_vrms(ADE_ReadData(VRMS, 3));
        screen1_data.Irms = ade_scale_irms(ADE_ReadData(IRMS, 3));
        lcd_screen_1_data_update(screen1_data);
    }

    if(pending & (1 << PIN_IRQ_IT)) {
        EXTI->PR |= 1 << PIN_IRQ_IT;	// clear pending flag, otherwise we'd get endless interrupts
        // handle pin I here
        uint32_t rststatus = ADE_ReadData(RSTSTATUS, 2);

        if(rststatus & (1 << IRQ_CYCEND)) {
        	screen2_data.ActivePower = ade_scale_power(ADE_ReadData(LAENERGY, 3));
        	screen2_data.ReactivePower = ade_scale_reactive_power(ADE_ReadData(LVARENERGY, 3));
        	screen2_data.ApparantPower = ade_scale_apparant_power(ADE_ReadData(LVAENERGY, 3));
        	screen2_data.PowerFactor = ade_get_power_factor();
        	lcd_screen_2_data_update(screen2_data);
        }

        if(rststatus & (1 << IRQ_PKV)) {

        }

        if(rststatus & (1 << IRQ_PKI)) {

		}
    }
}
