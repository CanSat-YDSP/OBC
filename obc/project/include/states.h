/*
 * states.h
 *
 * Created: 8/11/2025 10:12:19 pm
 *  Author: Yu Heng
 */ 


#ifndef STATES_H_
#define STATES_H_

#include <FreeRTOS.h>
#include "semphr.h"
#include "telemetry.h"

typedef enum {
	MODE_SIMULATION,
	MODE_FLIGHT
} CanSatMode_t;

typedef enum {
	LAUNCH_PAD,
	ASCENT,
	APOGEE,
	DESCENT,
	PROBE_RELEASE,
	LANDED
} CanSatStage_t;

extern SemaphoreHandle_t stateMutex;

extern TelemetryData universal_telemetry;

#endif /* STATES_H_ */