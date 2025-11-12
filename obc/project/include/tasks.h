/*
 * tasks.h
 *
 * Created: 18/8/2024 5:03:59 pm
 *  Author: huimin
 */ 

#ifndef BLINKY_TASK_H_
#define BLINKY_TASK_H_

#include <FreeRTOS.h>
#include "queue.h"

void task_led_blinky (void* pvParameters);
void simulated_data_reading (void *pvParameters);

extern QueueHandle_t simulated_pressure_queue;

#endif /* BLINKY_TASK_H_ */