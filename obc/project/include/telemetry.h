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

#include "states.h"

#define START_BYTE 0xFF
#define END_BYTE 0x0A

typedef struct {
	// =============== For Automation ===============
	float pressure;
	float altitude;
	CanSatMode_t mode;
	CanSatStage_t stage;
	// ==============================================
	uint8_t checksum;
} TelemetryData;

extern QueueHandle_t telemetryQueue;

extern TelemetryData universal_telemetry;

void send_to_ground (void *pvParameters);

#endif /* TELEMETRY_H_ */