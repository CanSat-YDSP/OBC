/*
 * telemetry.h
 *
 * Created: 8/11/2025 6:15:19 pm
 *  Author: Yu Heng
 */ 


#ifndef TELEMETRY_H_
#define TELEMETRY_H_

#include "FreeRTOS.h"
#include "queue.h"

#define START_BYTE 0xFF
#define END_BYTE 0x0A

typedef struct {
	// =============== For Automation ===============
	uint8_t placeholder1;
	uint8_t placeholder2;
	// ==============================================
	uint8_t checksum;
} TelemetryData;

extern QueueHandle_t telemetryQueue;

void send_to_ground (void *pvParameters);

#endif /* TELEMETRY_H_ */