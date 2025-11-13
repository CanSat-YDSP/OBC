/*
 * tasks.c
 *
 * Created: 6/11/2025 2:48:41 pm
 *  Author: Yu Heng
 */ 

#include <math.h>

#include <tasks.h>
#include <avr/io.h>

// FreeRTOS
#include <FreeRTOS.h>
#include <task.h>

#include "w25qxx.h"
#include "telemetry.h"
#include "states.h"
#include "uart.h"

#define FALL_THRESHOLD 2.0f
#define PARACHUTE_DEPLOY_ALT 700.0f
#define STATIONARY_THRESHOLD 0.5f

QueueHandle_t simulated_pressure_queue;
float old_altitude;

uint8_t is_falling(float new_alt) {
	return (old_altitude - new_alt > FALL_THRESHOLD);
}

uint8_t is_stationary(float new_alt) {
	return (fabsf(old_altitude - new_alt) < STATIONARY_THRESHOLD);
}

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
	old_altitude = 0;
	CanSatEvents_t event;
	while (1) {
		// read queue with pressure data
		if (xQueueReceive(simulated_pressure_queue, &pressure, portMAX_DELAY) == pdTRUE) {
			xSemaphoreTake(stateMutex, portMAX_DELAY);
			universal_telemetry.pressure = pressure;
			universal_telemetry.altitude = 44330 * (1-pow(pressure/101325, 0.1903));
			
			// send events if altitude is high enough
			if (universal_telemetry.stage == ASCENT && is_falling(universal_telemetry.altitude)) {
				event = MAX_HEIGHT_REACHED;
				xQueueSend(events_queue, &event, portMAX_DELAY);
			}
			
			if (universal_telemetry.stage == DESCENT && universal_telemetry.altitude < PARACHUTE_DEPLOY_ALT) {
				event = RELEASE_HEIGHT_REACHED;
				xQueueSend(events_queue, &event, portMAX_DELAY);
			}
			
			if (universal_telemetry.stage == PROBE_RELEASE && is_stationary(universal_telemetry.altitude)) {
				event = IS_LANDED;
				xQueueSend(events_queue, &event, portMAX_DELAY);
			}
			
			old_altitude = universal_telemetry.altitude;
			
			xSemaphoreGive(stateMutex);
		}
		
		vTaskDelay(pdMS_TO_TICKS(2000));
	}
}