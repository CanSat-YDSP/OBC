/*
 * tasks.c
 *
 * Created: 6/11/2025 2:48:41 pm
 *  Author: Yu Heng
 */ 

#include <tasks.h>
#include <avr/io.h>

// FreeRTOS
#include <FreeRTOS.h>

// This task lights up LED at digital pin 13 (built-in)
void task_led_blinky (void *pvParameters) {
	// Create tasks
	DDRB |= (1<<PB7); // built in LED
	PORTB |= (1<<PB7);
	
	while (1) {
		PORTB ^= (1<<PB7);
		vTaskDelay(pdMS_TO_TICKS(2000));
	}
}