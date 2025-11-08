#define F_CPU 16000000L // Specify oscillator frequency
#define BAUD_RATE 9600
#define UBBR ((F_CPU)/(16UL*BAUD_RATE) - 1) // for UART_init

#include <tasks.h>

// FreeRTOS
#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include <semphr.h>

// 
#include "uart.h"
#include "w25qxx.h"
#include "telemetry.h"
#include "states.h"

extern TaskHandle_t I2C_task;

int main(void)
{
	
	UART_init(UBBR);
	SPI_init();
	
	stateMutex = xSemaphoreCreateMutex();
	
	// Additional task to see that multiple tasks can run at the same time
	extern void simulated_data_reading (void *pvParameters);
	xTaskCreate(simulated_data_reading, "Task to simulate reading data from W25Q32, will include other sensors", 100, NULL, 2, NULL);
	
	// Task to send telemetry to ground
	extern void send_to_ground (void *pvParameters);
	xTaskCreate(send_to_ground, "Task to send telemetry to ground", 200, NULL, 2, NULL);
	
	
	
	// Start Scheduler
	vTaskStartScheduler();

	/* Execution will only reach here if there was insufficient heap to start the scheduler. */
	for ( ;; );
	
	return 0;
}
