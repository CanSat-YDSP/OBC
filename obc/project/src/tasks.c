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

QueueHandle_t simulated_pressure_queue;

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
	float pressure;
	while (1) {
		// read queue with pressure data
		if (xQueueReceive(simulated_pressure_queue, &pressure, portMAX_DELAY) == pdTRUE) {
			xSemaphoreTake(stateMutex, portMAX_DELAY);
			universal_telemetry.pressure = pressure;
			universal_telemetry.altitude = 10.0;
			
			// send events if altitude is high enough
			
			xSemaphoreGive(stateMutex);
		}
		
		vTaskDelay(pdMS_TO_TICKS(2000));
	}
}