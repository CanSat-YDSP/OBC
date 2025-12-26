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
	uint8_t software_ver;
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
	uint8_t cmd_echo;
	uint8_t app_checksum;
	UploadStatus upload_status;
	// ==============================================
	uint8_t checksum;
} TelemetryData;

typedef enum {
	RESET = 0x00,
	LAUNCH = 0x01,
	SET_PRESSURE = 0x02,
	ENTER_SIMULATION = 0x03,
	CALIBRATE_ALTITUDE = 0x04,
	BINARY_DATA_START = 0x05,
	BINARY_DATA_PACKET = 0x06,
	BINARY_DATA_END = 0x07,
	SERVO = 0x08,
	AT = 0x09,
	STARTUP_ACK = 0x0A
} CommandID;

extern QueueHandle_t telemetryQueue;

extern TelemetryData universal_telemetry;

extern uint8_t startup_ack;

void send_to_ground (void *pvParameters);

#endif /* TELEMETRY_H_ */