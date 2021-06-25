#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"

uint32_t SystemCoreClock = 16000000;

static void task1_handler(void* parameters);
static void task2_handler(void* parameters);

int main(void) {
	TaskHandle_t task1_handle;
	TaskHandle_t task2_handle;

	xTaskCreate(task1_handler, "Task-1", 200, NULL, 2, &task1_handle);
	xTaskCreate(task2_handler, "Task-2", 200, NULL, 2, &task2_handle);
	vTaskStartScheduler();
	while (1);
}

static void task1_handler(void* parameters) {
	int count = 0;
	while (1) {
		printf("task1 %d\n", count);
		count++;
		vTaskDelay(1000);
		taskYIELD();
	}
}

static void task2_handler(void* parameters) {
	int count = 0;
	while (1) {
		printf("task2 %d\n", count);
		count++;
		vTaskDelay(1000);
		taskYIELD();
	}
}
