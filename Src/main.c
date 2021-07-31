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

#define KEYPAD_HOLD_TIMEOUT		2000

#define SQRT_2					1.414213562

uint32_t SystemCoreClock = 16000000;

LCD_Data_Screen1_t screen1_data;
LCD_Data_Screen2_t screen2_data;
LCD_Data_Screen3_t screen3_data;
LCD_Data_Screen4_t screen4_data;

uint16_t interval_timer = 0, user_interval_set;

QueueHandle_t ade_queue_handle;
QueueHandle_t keypad_queue_handle;
QueueHandle_t rtc_queue_handle;
QueueHandle_t led_queue_handle;

typedef enum {
	R_ON,
	R_OFF,
	G_ON,
	G_OFF,
	B_ON,
	B_OFF
} LED_Control_t;

void lcd_handler(void* parameters);
void ade_handler(void* parameters);
void keypad_handler(void* parameters);
void rtc_handler(void* parameters);
void usart_handler(void* parameters);
void led_handler(void* parameters);

int main(void) {

	xTaskCreate(lcd_handler, "LCD5110", 2048, NULL, 1, NULL);
	xTaskCreate(ade_handler, "ADE7753", 512, NULL, 1, NULL);
	xTaskCreate(keypad_handler, "Keypad", 256, NULL, 1, NULL);
	xTaskCreate(rtc_handler, "DS1307", 512, NULL, 1, NULL);
	xTaskCreate(usart_handler, "USART", 512, NULL, 1, NULL);
	xTaskCreate(led_handler, "LED", 128, NULL, 1, NULL);

	ade_queue_handle = xQueueCreate(20, sizeof(ADE_INT_t));
	keypad_queue_handle = xQueueCreate(5, sizeof(KEYPAD_Button_t));
	rtc_queue_handle = xQueueCreate(1, sizeof(DS1307_DateTime_t));
	led_queue_handle = xQueueCreate(10, sizeof(LED_Control_t));

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

	screen4_data.User_Interval = Sample_Interval_60;

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
											screen4_data = lcd_screen_4_commit_parameters();
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
	LED_Control_t led_ctrl;
	ADE_Event_t ade_event;
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
	ADE_WriteData(LINECYC, 0x00c8, 2);

//	Sag Cycle: 3
	ADE_WriteData(SAGCYC, 0x04, 1);

//	Sag level
	ADE_WriteData(SAGLVL, 0x00, 1);

//	Vpeak level
	ADE_WriteData(VPKLVL, 0xFF, 1);

//	Ipeak level
	ADE_WriteData(IPKLVL, 0xFF, 1);

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
					case ADE_INT_IRQ:
						vTaskDelay(1);
						rststatus = ADE_ReadData(RSTSTATUS, 2);

						if(rststatus & (1 << IRQ_SAG)) {
							led_ctrl = B_ON;
							xQueueSend(led_queue_handle, (void *) &led_ctrl, (TickType_t) 0);
						}

						if(rststatus & (1 << IRQ_CYCEND)) {
							screen2_data.ActivePower = ade_scale_power(ADE_ReadData(LAENERGY, 3));
							screen2_data.ReactivePower = ade_scale_reactive_power(ADE_ReadData(LVARENERGY, 3));
							screen2_data.ApparantPower = ade_get_apparant_power();
							screen2_data.PowerFactor = ade_get_power_factor();
							lcd_screen_2_data_update(screen2_data);
						}

						if(rststatus & (1 << IRQ_PKV)) {
							led_ctrl = R_ON;
							xQueueSend(led_queue_handle, (void *) &led_ctrl, (TickType_t) 0);
						}

						if(rststatus & (1 << IRQ_PKI)) {
							led_ctrl = R_ON;
							xQueueSend(led_queue_handle, (void *) &led_ctrl, (TickType_t) 0);
						}

//						ADE_ReadData(RSTSTATUS, 2);
						break;

					case ADE_INT_ZX:
						screen1_data.Vrms = ade_scale_vrms(ADE_ReadData(VRMS, 3));
						screen1_data.Irms = ade_scale_irms(ADE_ReadData(IRMS, 3));
						screen1_data.Vpeak = screen1_data.Vrms * SQRT_2;
						screen1_data.Ipeak = screen1_data.Irms * SQRT_2;
						lcd_screen_1_data_update(screen1_data);

						screen3_data.ActiveEnergy = ade_accumulate_scale_energy(ADE_ReadData(RAENERGY, 3));
						lcd_screen_3_data_update(screen3_data);

						led_ctrl = G_ON;
						xQueueSend(led_queue_handle, (void *) &led_ctrl, (TickType_t) 0);
						break;

					case ADE_INT_SAG:

						break;

					case ADE_USER_CONFIG:
						ADE_WriteData(VPKLVL, rescale_user_pkv_to_hex(screen4_data.User_PKV), 1);
						ADE_WriteData(IPKLVL, rescale_user_pkv_to_hex(screen4_data.User_PKI), 1);
						ADE_WriteData(SAGLVL, rescale_user_pkv_to_hex(screen4_data.User_SAG), 1);
						screen4_data.User_PKV = rescale_hex_to_user_pkv(ADE_ReadData(VPKLVL, 1));
						screen4_data.User_PKI = rescale_hex_to_user_pki(ADE_ReadData(IPKLVL, 1));
						screen4_data.User_SAG = rescale_hex_to_user_sag(ADE_ReadData(SAGLVL, 1));
						lcd_screen_4_data_update(screen4_data);
						break;
					default:
						break;
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
				xQueueSend(keypad_queue_handle, (void*) &Keypad_Hold_Button, (TickType_t) 0);
			}
		}
		if(Keypad_Button == KEYPAD_NOPRESSED && Keypad_prev != KEYPAD_NOPRESSED)
			if(hold_timeout > 0)
				xQueueSend(keypad_queue_handle, (void*) &Keypad_prev, (TickType_t) 0);
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
			if(xQueueReceive(rtc_queue_handle, &rtc_config_data, (TickType_t) 5))
				DS1307_SetDateTime(&rtc_config_data);

		DS1307_GetDateTime(&rtc_datetime);
		sec = rtc_datetime.seconds;
		if((sec - sec_prev == 1) || (sec_prev - sec == 59)) {
			lcd_screen_3_timer_count_up();
			interval_timer++;
		}
		lcd_screen_4_rtc_update(rtc_datetime);

		sec_prev = sec;
		vTaskDelay(250);
		taskYIELD();
	}
}

void usart_handler(void* parameters) {

// uart init

	while(1) {
		if(interval_timer >= screen4_data.User_Interval*60) {
//	uart send data
			interval_timer = 0;
		}
		vTaskDelay(500);
		taskYIELD();
	}
}

void led_handler(void* parameters) {
	uint16_t led_timeout = 20;

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

	LED_Control_t led;

	uint8_t r_led_count = 0, g_led_count = 0, b_led_count = 0;

	while(1) {
		if(led_queue_handle != NULL) {
			if(xQueueReceive(led_queue_handle, &led, (TickType_t) 100)) {
				switch (led) {
					case R_ON:
						r_led_count = led_timeout;
						break;
					case G_ON:
						g_led_count = led_timeout;
						break;
					case B_ON:
						b_led_count = led_timeout;
						break;
					case R_OFF:
						r_led_count = 0;
						break;
					case G_OFF:
						g_led_count = 0;
						break;
					case B_OFF:
						b_led_count = 0;
						break;
					default:
						break;
				}
			}
		}

		if(r_led_count) {
			R_LED_ON;
			r_led_count--;
		}
		else R_LED_OFF;

		if(g_led_count) {
			G_LED_ON;
			g_led_count--;
		}
		else G_LED_OFF;

		if(b_led_count) {
			B_LED_ON;
			b_led_count--;
		}
		else B_LED_OFF;

		taskYIELD();
	}
}

void EXTI15_10_IRQHandler(void) {
	static uint8_t zx_count = 0;
	ADE_Event_t ade_int;
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
