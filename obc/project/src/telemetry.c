/*
 * telemetry.c
 *
 * Created: 8/11/2025 6:20:58 pm
 *  Author: Yu Heng
 */

#include <string.h>

#include <FreeRTOS.h>
#include <queue.h>
#include <task.h>

#include "telemetry.h"
#include "uart.h"
#include "states.h"
#include "tasks.h"

QueueHandle_t telemetryQueue;
uint8_t packet_count = 0;

uint8_t checksum_checker(uint8_t *buf, size_t len) {
    uint8_t checksum = 0;
    for (size_t i = 0; i < len; i++) {
        checksum ^= buf[i];
    }
    return checksum;
}

uint8_t checksum_calculator(TelemetryData *tm_data)
{
	uint8_t checksum = 0;

	// =============== For Automation ===============
	for (size_t i = 0; i < sizeof(tm_data->packet_count); i++)
	{
		checksum ^= *((uint8_t *)(&(tm_data->packet_count)) + i);
	}
	
	for (size_t i = 0; i < sizeof(tm_data->mode); i++)
	{
		checksum ^= *((uint8_t *)(&(tm_data->mode)) + i);
	}
	
	for (size_t i = 0; i < sizeof(tm_data->stage); i++)
	{
		checksum ^= *((uint8_t *)(&(tm_data->stage)) + i);
	}
	
	for (size_t i = 0; i < sizeof(tm_data->altitude); i++)
	{
		checksum ^= *((uint8_t *)(&(tm_data->altitude)) + i);
	}
	
	for (size_t i = 0; i < sizeof(tm_data->pressure); i++)
	{
		checksum ^= *((uint8_t *)(&(tm_data->pressure)) + i);
	}
	// ==============================================

	return checksum;
}

void send_to_ground(void *pvParameters)
{
	uint8_t start = START_BYTE;
	uint8_t end = END_BYTE;
	while (1)
	{
		xSemaphoreTake(stateMutex, portMAX_DELAY);

		universal_telemetry.packet_count = packet_count++;
		// calculate checksum
		universal_telemetry.checksum = checksum_calculator(&universal_telemetry);

		UART1_send_bytes(&start, 1);

		// =============== For Automation ===============
		UART1_send_bytes(&(universal_telemetry.packet_count), sizeof(universal_telemetry.packet_count));
		UART1_send_bytes(&(universal_telemetry.mode), sizeof(universal_telemetry.mode));
		UART1_send_bytes(&(universal_telemetry.stage), sizeof(universal_telemetry.stage));
		UART1_send_bytes(&(universal_telemetry.altitude), sizeof(universal_telemetry.altitude));
		UART1_send_bytes(&(universal_telemetry.pressure), sizeof(universal_telemetry.pressure));
		// ==============================================

		UART1_send_bytes(&(universal_telemetry.checksum), sizeof(universal_telemetry.checksum));
		UART1_send_bytes(&end, 1);

		xSemaphoreGive(stateMutex);

		vTaskDelay(pdMS_TO_TICKS(1000));
	}
}

void receive_from_ground(void *pvParameters) {
	uint8_t buf[20];
	while (1) {
		
		UART1_receive_bytes(buf);

		// =============== For Automation ===============

		size_t command_length = buf[0];
		uint8_t command_id = buf[1];
		CanSatEvents_t event;
		
		// confirm validity of command packet
		if (checksum_checker(&buf[1], command_length) == buf[command_length + 1]) {
			switch (command_id) {
			case 0x01:
				event = LAUNCH_OK;
				xQueueSend(events_queue, &event, portMAX_DELAY);
				break;
			case 0x02: ;
				float pressure;
				memcpy(&pressure, &buf[2], sizeof(float));
				xQueueSend(simulated_pressure_queue, &pressure, portMAX_DELAY);
				break;
			case 0x03:
				event = ENTER_SIM;
				xQueueSend(events_queue, &event, portMAX_DELAY);
				break;
			default:
				print("Something went wrong!\r\n");
				UART0_send_bytes(buf, 20);
				break;
			}
		}
		else {
			print("Dropped an invalid packet.\r\n");
		}

		vTaskDelay(pdMS_TO_TICKS(1000));
	}
}