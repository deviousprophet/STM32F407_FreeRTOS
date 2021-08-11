#include "main.h"

#ifdef USE_MAIN_C

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

Monitor_Data_t monitor_data;

uint16_t interval_timer = 0;

QueueHandle_t ade_queue_handle;
QueueHandle_t keypad_queue_handle;
QueueHandle_t rtc_queue_handle;

void lcd_handler(void* parameters);
void ade_handler(void* parameters);
void keypad_handler(void* parameters);
void rtc_handler(void* parameters);
void usart_handler(void* parameters);

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

	memset(&monitor_data, 0, sizeof(Monitor_Data_t));
	monitor_data.data4.User_Interval = Sample_Interval_5;

	xTaskCreate(lcd_handler, "LCD5110", 2048, NULL, 1, NULL);
	xTaskCreate(ade_handler, "ADE7753", 512, NULL, 1, NULL);
	xTaskCreate(keypad_handler, "Keypad", 256, NULL, 1, NULL);
	xTaskCreate(rtc_handler, "DS1307", 512, NULL, 1, NULL);
	xTaskCreate(usart_handler, "USART", 512, NULL, 1, NULL);

	ade_queue_handle = xQueueCreate(20, sizeof(ADE_Event_t));
	keypad_queue_handle = xQueueCreate(5, sizeof(KEYPAD_Button_t));
	rtc_queue_handle = xQueueCreate(1, sizeof(DS1307_DateTime_t));

	vTaskStartScheduler();

	while(1);
	return 0;
}

void lcd_handler(void* parameters) {
	LCD5110_Init(0x37);
	lcd_screen_1_clear();
	lcd_screen_2_clear();
	lcd_screen_3_clear();
	lcd_screen_4_clear();

	KEYPAD_Button_t keypad;
	LCD_Screen_t screen = LCD_Screen_1;

	while(1) {
		if(keypad_queue_handle != NULL) {
			if(xQueueReceive(keypad_queue_handle, &keypad, (TickType_t) 5)) {
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
							if(screen == LCD_Screen_3) {
								lcd_screen_3_clear();
								reset_energy();
								memset(&monitor_data.data3, 0, sizeof(LCD_Data_Screen3_t));
							}
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
											monitor_data.data4 = lcd_screen_4_commit_parameters();
											ADE_Event_t ade_event = ADE_USER_CONFIG;
											xQueueSend(ade_queue_handle, &ade_event, 0);
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
	ADE_Event_t ade_event;
	uint32_t status;

	ADE_Init();

/* CH1 full-scale 0.125V
 * PGA1 x16
 * PGA2 x2
 */
	ADE_SetGain(FULL_SCALE_0125, GAIN_8, GAIN_2);

/* set POAM, CYCMODE
 * clear DISSAG
 */
	ADE_WriteData(MODE,
			(0x000c
			& ~(1 << MODE_DISSAG))
			| (1 << MODE_CYCMODE)
			| (1 << MODE_POAM),
			2);

/* LINECYC = 200 */
	ADE_WriteData(LINECYC, 0x00c8, 2);

/* Sag Cycle: 3 */
	ADE_WriteData(SAGCYC, 0x04, 1);

/* Sag V-peak I-peak level */
	ADE_WriteData(SAGLVL, 0x00, 1);
	ADE_WriteData(VPKLVL, 0x4A, 1);
	ADE_WriteData(IPKLVL, 0x50, 1);

/* Re-scale from register values to user values */
	monitor_data.data4.User_PKV = rescale_hex_to_user_pkv(ADE_ReadData(VPKLVL, 1));
	monitor_data.data4.User_PKI = rescale_hex_to_user_pki(ADE_ReadData(IPKLVL, 1));
	monitor_data.data4.User_SAG = rescale_hex_to_user_sag(ADE_ReadData(SAGLVL, 1));
	lcd_screen_4_data_update(monitor_data.data4);

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
						monitor_data.data1.Vrms = ade_scale_vrms(ADE_ReadData(VRMS, 3));
						monitor_data.data1.Irms = ade_scale_irms(ADE_ReadData(IRMS, 3));
						monitor_data.data1.Vpeak = monitor_data.data1.Vrms * sqrtf(2);
						monitor_data.data1.Ipeak = monitor_data.data1.Irms * sqrtf(2);
						lcd_screen_1_data_update(monitor_data.data1);

						monitor_data.data3.ActiveEnergy = ade_accumulate_scale_energy(ADE_ReadData(RAENERGY, 3));
						lcd_screen_3_data_update(monitor_data.data3);

						status = ADE_ReadData(RSTSTATUS, 2);

						if(status & (1 << IRQ_CYCEND)) {
							monitor_data.data2.ActivePower = ade_scale_power(ADE_ReadData(LAENERGY, 3));
							monitor_data.data2.ReactivePower = ade_scale_reactive_power(ADE_ReadData(LVARENERGY, 3));
							monitor_data.data2.ApparantPower = ade_get_apparant_power();
							monitor_data.data2.PowerFactor = ade_get_power_factor();
							lcd_screen_2_data_update(monitor_data.data2);
						}

						if(status & (1 << IRQ_SAG))
							B_LED_ON;
						else B_LED_OFF;

						if(status & ((1 << IRQ_PKV) | (1 << IRQ_PKI)))
							R_LED_ON;
						else R_LED_OFF;

						G_LED_ON;
						break;

					case ADE_INT_SAG:

						break;

					case ADE_USER_CONFIG:
						ADE_WriteData(VPKLVL, rescale_user_pkv_to_hex(monitor_data.data4.User_PKV), 1);
						ADE_WriteData(IPKLVL, rescale_user_pki_to_hex(monitor_data.data4.User_PKI), 1);
						ADE_WriteData(SAGLVL, rescale_user_sag_to_hex(monitor_data.data4.User_SAG), 1);
						monitor_data.data4.User_PKV = rescale_hex_to_user_pkv(ADE_ReadData(VPKLVL, 1));
						monitor_data.data4.User_PKI = rescale_hex_to_user_pki(ADE_ReadData(IPKLVL, 1));
						monitor_data.data4.User_SAG = rescale_hex_to_user_sag(ADE_ReadData(SAGLVL, 1));

						lcd_screen_4_data_update(monitor_data.data4);
						break;
					default:
						break;
				}
			}
		}
		taskYIELD();
	}
}

void keypad_handler(void* parameters) {
	KEYPAD_Init(KEYPAD_Type_Large);

	KEYPAD_Button_t Keypad_Hold_Button, Keypad_Button, Keypad_prev = KEYPAD_NOPRESSED;
	TickType_t update_delay = 10;
	int hold_timeout = KEYPAD_HOLD_TIMEOUT / (uint8_t) update_delay;

	while(1) {
		KEYPAD_Update();
		Keypad_Button = KEYPAD_Read();

		if(Keypad_Button != KEYPAD_NOPRESSED && Keypad_Button == Keypad_prev) {
			if(hold_timeout > 0) hold_timeout--;
			else if (!hold_timeout) {
				hold_timeout--;
				Keypad_Hold_Button = KEYPAD_Hold_Button(Keypad_Button);
				xQueueSend(keypad_queue_handle, &Keypad_Hold_Button, 0);
			}
		}
		if(Keypad_Button == KEYPAD_NOPRESSED && Keypad_prev != KEYPAD_NOPRESSED)
			if(hold_timeout > 0)
				xQueueSend(keypad_queue_handle, &Keypad_prev, 0);
		if(Keypad_Button == KEYPAD_NOPRESSED && Keypad_prev != KEYPAD_NOPRESSED)
			hold_timeout = KEYPAD_HOLD_TIMEOUT / (uint8_t) update_delay;

		Keypad_prev = Keypad_Button;
		vTaskDelay(update_delay);
		taskYIELD();
	}
}

void rtc_handler(void* parameters) {
	DS1307_DateTime_t rtc_datetime, rtc_config_data;
	int sec, sec_prev = 0;

	while(DS1307_Init() != DS1307_Result_OK) vTaskDelay(10);

	while(1) {
		if(rtc_queue_handle != NULL)
			if(xQueueReceive(rtc_queue_handle, &rtc_config_data, 5))
				DS1307_SetDateTime(&rtc_config_data);

		DS1307_GetDateTime(&rtc_datetime);
		sec = rtc_datetime.seconds;
		if((sec - sec_prev == 1) || (sec_prev - sec == 59)) {
			monitor_data.data3.EnergyTimer.seconds++;
			if(monitor_data.data3.EnergyTimer.seconds > 59) {
				monitor_data.data3.EnergyTimer.seconds -= 60;
				monitor_data.data3.EnergyTimer.minutes++;
				if(monitor_data.data3.EnergyTimer.minutes > 59) {
					monitor_data.data3.EnergyTimer.minutes -= 60;
					monitor_data.data3.EnergyTimer.hours++;
					if(monitor_data.data3.EnergyTimer.hours > 23) {
						monitor_data.data3.EnergyTimer.hours -= 24;
						monitor_data.data3.EnergyTimer.days++;
					}
				}
			}
			lcd_screen_3_data_update(monitor_data.data3);
			interval_timer++;
		}
		lcd_screen_4_rtc_update(rtc_datetime);
		monitor_data.datetime = rtc_datetime;

		sec_prev = sec;
		vTaskDelay(250);
		taskYIELD();
	}
}

void usart_handler(void* parameters) {

	GPIO_Handle_t uart_gpio;
	uart_gpio.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	uart_gpio.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
	uart_gpio.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PIN_PU;
	uart_gpio.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
	uart_gpio.GPIO_PinConfig.GPIO_PinAltFunMode = 7;

//	USART TX
	uart_gpio.pGPIOx = GPIOB;
	uart_gpio.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_6;
	GPIO_Init(&uart_gpio);

//	USART RX
	uart_gpio.pGPIOx = GPIOA;
	uart_gpio.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_10;
	GPIO_Init(&uart_gpio);


	USART_Handle_t uart_handle;
	uart_handle.pUSARTx = USART1;
	uart_handle.USART_Config.USART_Baud = USART_STD_BAUD_115200;
	uart_handle.USART_Config.USART_HWFlowControl = USART_HW_FLOW_CTRL_NONE;
	uart_handle.USART_Config.USART_Mode = USART_MODE_ONLY_TX;
	uart_handle.USART_Config.USART_NoOfStopBits = USART_STOPBITS_1;
	uart_handle.USART_Config.USART_WordLength = USART_WORDLEN_8BITS;
	uart_handle.USART_Config.USART_ParityControl = USART_PARITY_DISABLE;
	USART_Init(&uart_handle);

	USART_PeripheralControl(USART1, ENABLE);

	uint8_t* uart_buff = malloc(FRAME_MONITOR_BUFFER_SIZE);

	while(1) {
#ifndef INTERVAL_SECOND
		if(interval_timer >= monitor_data.data4.User_Interval*60) {
#else
		if(interval_timer >= monitor_data.data4.User_Interval) {
#endif
			ade_data_to_frame_uart_send(monitor_data, uart_buff);
			USART_SendData(&uart_handle, uart_buff, FRAME_MONITOR_BUFFER_SIZE);
			interval_timer = 0;
		}
		vTaskDelay(500);
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
