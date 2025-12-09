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

extern uint8_t packet_count;

typedef struct {
	// =============== For Automation ===============
	uint8_t packet_count;
	CanSatMode_t mode;
	CanSatStage_t stage;
	float altitude;
	float pressure;
	float temperature;
	float acc_x;
	float acc_y;
	float acc_z;
	float mag_x;
	float mag_y;
	float mag_z;
	float gyr_x;
	float gyr_y;
	float gyr_z;
	// ==============================================
	uint8_t checksum;
} TelemetryData;

extern QueueHandle_t telemetryQueue;

extern TelemetryData universal_telemetry;

void send_to_ground (void *pvParameters);

#endif /* TELEMETRY_H_ */