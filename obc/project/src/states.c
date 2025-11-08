/*
 * states.c
 *
 * Created: 8/11/2025 10:12:27 pm
 *  Author: Yu Heng
 */ 

#include <FreeRTOS.h>
#include "semphr.h"

#include "telemetry.h"

SemaphoreHandle_t stateMutex;
TelemetryData universal_telemetry;