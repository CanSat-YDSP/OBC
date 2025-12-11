/*
 * states.c
 *
 * Created: 8/11/2025 10:12:27 pm
 *  Author: Yu Heng
 */

#include <FreeRTOS.h>
#include <util/delay.h>
#include "semphr.h"

#include "telemetry.h"
#include "uart.h"
#include "tasks.h"
#include "servo.h"

SemaphoreHandle_t stateMutex;
TelemetryData universal_telemetry;

QueueHandle_t events_queue;

float deploy_height = 0;

HandlerFunc functionTable[] = {
	ascent_handler,
	descent_hander,
	release_handler,
	landing_handler,
	sim_handler,
	calibration_handler};

void ascent_handler() {
	xSemaphoreTake(stateMutex, portMAX_DELAY);
	if (universal_telemetry.stage == LAUNCH_PAD) {
		universal_telemetry.stage = ASCENT;
		print("Launched!\r\n");
	}
	xSemaphoreGive(stateMutex);
}

void descent_hander() {
	xSemaphoreTake(stateMutex, portMAX_DELAY);
	if (universal_telemetry.stage == ASCENT) {
		universal_telemetry.stage = DESCENT;
		print("Falling!\r\n");
		// set deploy height to ~75% height reached
		deploy_height = 0.75 * universal_telemetry.altitude;
		// print("deploy_height established!\r\n");
	}
	xSemaphoreGive(stateMutex);
}

void release_handler() {
	xSemaphoreTake(stateMutex, portMAX_DELAY);
	if (universal_telemetry.stage == DESCENT) {
		print("release handler\r\n");
		universal_telemetry.stage = PROBE_RELEASE;
		print("Probe Released!\r\n");
		move_servo();
		_delay_ms(1000);
		stop_servo();
	}
	xSemaphoreGive(stateMutex);;
}

void landing_handler() {
	xSemaphoreTake(stateMutex, portMAX_DELAY);
	if (universal_telemetry.stage == PROBE_RELEASE) {
		universal_telemetry.stage = LANDED;
		print("Landed!\r\n");
		DDRL |= (1<<PL5);
		PORTL |= (1<<PL5);
	}
	xSemaphoreGive(stateMutex);;
}

void sim_handler() {
	xSemaphoreTake(stateMutex, portMAX_DELAY);
	if (universal_telemetry.mode != MODE_SIMULATION) {
		universal_telemetry.mode = MODE_SIMULATION;
		// extern void simulated_data_reading (void *pvParameters);
		// xTaskCreate(simulated_data_reading, "Task to simulate reading data from W25Q32, will include other sensors", 100, NULL, 2, NULL);
	}
	xSemaphoreGive(stateMutex);
}

void calibration_handler() {
	float pressure, temperature;
	BMP390_get_readings(&pressure, &temperature);
	old_altitude = 0;
	calibrated_altitude = 44330.0f * (1.0f-powf(pressure/CURRENT_PRESSURE, 0.1903f));
}

void state_manager(void *pvParameters) {
	universal_telemetry.mode = MODE_FLIGHT; // default: MODE_FLIGHT
	universal_telemetry.stage = LAUNCH_PAD;		// cansat starts off on ground

	CanSatEvents_t current_event;

	while (1) {
		if (xQueueReceive(events_queue, &current_event, portMAX_DELAY) == pdTRUE) {
			functionTable[current_event]();
		}
	}
}
