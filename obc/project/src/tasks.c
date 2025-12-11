/*
 * tasks.c
 *
 * Created: 6/11/2025 2:48:41 pm
 *  Author: Yu Heng
 */ 

#include <math.h>

#include <tasks.h>
#include <avr/io.h>
#include <util/delay.h>

// FreeRTOS
#include <FreeRTOS.h>
#include <task.h>

#include "w25qxx.h"
#include "telemetry.h"
#include "states.h"
#include "uart.h"
#include "BMP390.h"
#include "BNO055.h"

#define FALL_THRESHOLD 5.0f
#define STATIONARY_THRESHOLD 0.5f

QueueHandle_t simulated_pressure_queue;
float old_altitude;
float calibrated_altitude = 0;

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
		vTaskDelay(pdMS_TO_TICKS(1000));
	}
}

void data_reading (void *pvParameters) {
	float pressure;
	float temperature;
	
	float acc[3];
	float mag[3];
	float gyr[3];
	
	old_altitude = 0.0f;
	CanSatEvents_t event;
	while (1) {
		// read queue with pressure data
		xSemaphoreTake(stateMutex, portMAX_DELAY);
		
		_delay_ms(100); // must delay
		
		BMP390_get_readings(&pressure, &temperature);
		if (universal_telemetry.mode == MODE_SIMULATION) {
			if (xQueueReceive(simulated_pressure_queue, &pressure, portMAX_DELAY) != pdTRUE) {
				xSemaphoreGive(stateMutex);
				return 0;
			}
		}
			
		universal_telemetry.pressure = pressure;
		universal_telemetry.altitude = 44330.0f * (1.0f-powf(pressure/CURRENT_PRESSURE, 0.1903f));
		universal_telemetry.altitude -= calibrated_altitude;
		
		universal_telemetry.temperature = temperature;
		
		//BNO055_read_acc(acc);
		//BNO055_read_mag(mag);
		//BNO055_read_gyr(gyr);
		
		universal_telemetry.acc_x = acc[0];
		universal_telemetry.acc_y = acc[1];
		universal_telemetry.acc_z = acc[2];
		
		universal_telemetry.mag_x = mag[0];
		universal_telemetry.mag_y = mag[1];
		universal_telemetry.mag_z = mag[2];
		
		universal_telemetry.gyr_x = gyr[0];
		universal_telemetry.gyr_y = gyr[1];
		universal_telemetry.gyr_z = gyr[2];
			
		char altitude_text[20];
		sprintf(altitude_text, "Altitude: %d\r\n", (int)(universal_telemetry.altitude * 100));
		print(altitude_text);
			
		// send events if altitude is high enough
		if (is_falling(universal_telemetry.altitude)) {
			print("Is Falling!\r\n");
			event = MAX_HEIGHT_REACHED;
			xQueueSend(events_queue, &event, portMAX_DELAY);
		}
			
		if (universal_telemetry.altitude < deploy_height) {
			event = RELEASE_HEIGHT_REACHED;
			xQueueSend(events_queue, &event, portMAX_DELAY);
		}
			
		if (is_stationary(universal_telemetry.altitude)) {
			event = IS_LANDED;
			xQueueSend(events_queue, &event, portMAX_DELAY);
		}
			
		old_altitude = universal_telemetry.altitude;
			
		xSemaphoreGive(stateMutex);
		
		vTaskDelay(pdMS_TO_TICKS(1000));
	}
}