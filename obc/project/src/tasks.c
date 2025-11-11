/*
 * tasks.c
 *
 * Created: 6/11/2025 2:48:41 pm
 *  Author: Yu Heng
 */ 

#include <tasks.h>
#include <avr/io.h>

// FreeRTOS
#include <FreeRTOS.h>
#include <task.h>

#include "w25qxx.h"
#include "telemetry.h"
#include "states.h"
#include "uart.h"

// This task lights up LED at digital pin 13 (built-in)
void task_led_blinky (void *pvParameters) {
	// Create tasks
	DDRB |= (1<<PB7); // built in LED
	PORTB |= (1<<PB7);
	
	while (1) {
		PORTB ^= (1<<PB7);
		vTaskDelay(pdMS_TO_TICKS(2000));
	}
}

void simulated_data_reading (void *pvParameters) {
	while (1) {
		uint8_t success = W25QXX_test();
		
		//char success_str[2];
		//memcpy(success_str, &success, 1);
		//success_str[1] = '\0';
		//print(success_str);
		
		xSemaphoreTake(stateMutex, portMAX_DELAY);
		universal_telemetry.placeholder1 = 'm';
		universal_telemetry.placeholder2 = success;
		xSemaphoreGive(stateMutex);
		
		print("Read Sensors\r\n");
		
		vTaskDelay(pdMS_TO_TICKS(2000));
	}
}