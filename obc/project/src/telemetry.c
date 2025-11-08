/*
 * telemetry.c
 *
 * Created: 8/11/2025 6:20:58 pm
 *  Author: Yu Heng
 */ 

#include <FreeRTOS.h>
#include <queue.h>
#include "telemetry.h"
#include "uart.h"
#include "states.h"

QueueHandle_t telemetryQueue;

uint8_t checksum_calculator (TelemetryData *tm_data) {
	uint8_t checksum = 0;
	
	// =============== For Automation ===============
	for (size_t i = 0; i < sizeof(tm_data->placeholder1); i++) {
		checksum ^= *((uint8_t*)(&(tm_data->placeholder1)) + i);
	}
	
	for (size_t i = 0; i < sizeof(tm_data->placeholder2); i++) {
		checksum ^= *((uint8_t*)(&(tm_data->placeholder2)) + i);
	}
	// ==============================================
	
	return checksum;
}

void send_to_ground (void *pvParameters) {
	uint8_t start = START_BYTE;
	uint8_t end = END_BYTE;
	while (1) {
		xSemaphoreTake(stateMutex, portMAX_DELAY);
			
		// calculate checksum
		universal_telemetry.checksum = checksum_calculator(&universal_telemetry);
						
		UART1_send_bytes(&start, 1);
			
		// =============== For Automation ===============
		UART1_send_bytes(&(universal_telemetry.placeholder1), sizeof(universal_telemetry.placeholder1)); 
		UART1_send_bytes(&(universal_telemetry.placeholder2), sizeof(universal_telemetry.placeholder2));
		// ==============================================
			
		UART1_send_bytes(&(universal_telemetry.checksum), sizeof(universal_telemetry.checksum));
		UART1_send_bytes(&end, 1);
		
		vTaskDelay(pdMS_TO_TICKS(2000));
	}
}

void receive_from_ground (void *pvParameters) {
	while (1) {
		
	}
}