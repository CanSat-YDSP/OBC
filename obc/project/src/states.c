/*
 * states.c
 *
 * Created: 8/11/2025 10:12:27 pm
 *  Author: Yu Heng
 */

#include <FreeRTOS.h>
#include "semphr.h"

#include "telemetry.h"
#include "uart.h"

SemaphoreHandle_t stateMutex;
TelemetryData universal_telemetry;

QueueHandle_t events_queue;

HandlerFunc functionTable[] = {
	ascent_handler};

void ascent_handler()
{
	universal_telemetry.stage = ASCENT;

	print("Launched!");
}

void state_manager(void *pvParameters)
{
	universal_telemetry.mode = MODE_SIMULATION; // hard code until all sensors come
	universal_telemetry.stage = LAUNCH_PAD;		// cansat starts off on ground

	CanSatEvents_t current_event;

	while (1)
	{
		if (xQueueReceive(events_queue, &current_event, portMAX_DELAY) == pdTRUE)
		{
			functionTable[current_event]();
		}
	}
}
