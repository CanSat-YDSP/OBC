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

typedef enum {
	MODE_SIMULATION,
	MODE_FLIGHT
} CanSatMode_t;

typedef enum {
	LAUNCH_PAD,
	ASCENT,
	// APOGEE,
	DESCENT,
	PROBE_RELEASE,
	LANDED
} CanSatStage_t;

typedef enum {
	LAUNCH_OK,
	MAX_HEIGHT_REACHED,
	RELEASE_HEIGHT_REACHED,
	IS_LANDED,
	ENTER_SIM
} CanSatEvents_t;

typedef void (*HandlerFunc)(); // typedef for function table

// extern all handler functions
extern void ascent_handler();
extern void descent_hander();
extern void release_handler();
extern void landing_handler();
extern void sim_handler();

extern HandlerFunc functionTable[];
extern SemaphoreHandle_t stateMutex;

extern QueueHandle_t events_queue;

extern float deploy_height;

#endif /* STATES_H_ */