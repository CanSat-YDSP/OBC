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
#include "spi.h"
#include "w25qxx.h"
#include "telemetry.h"
#include "states.h"

#define RX_QUEUE_LEN 30
#define EVENTS_QUEUE_LEN 10
#define SIM_PRESSURE_LEN 10

int main(void)
{
	
	sei();
	
	UART_init(UBBR);
	SPI_init();
	
	stateMutex = xSemaphoreCreateMutex(); // mutex for universal telemetry; in hindsight could also use a queue since state_handler doesn't use it
	uart1_rx_queue = xQueueCreate(RX_QUEUE_LEN, sizeof(uint8_t));
	events_queue = xQueueCreate(EVENTS_QUEUE_LEN, sizeof(CanSatEvents_t));
	simulated_pressure_queue = xQueueCreate(SIM_PRESSURE_LEN, sizeof(float));
	
	// Additional task to see that multiple tasks can run at the same time
	extern void simulated_data_reading (void *pvParameters);
	xTaskCreate(simulated_data_reading, "Task to simulate reading data from W25Q32, will include other sensors", 100, NULL, 2, NULL);
	
	extern void send_to_ground (void *pvParameters);
	xTaskCreate(send_to_ground, "Task to send telemetry to ground", 200, NULL, 2, NULL);
	
	extern void receive_from_ground (void *pvParameters);
	xTaskCreate(receive_from_ground, "Task to receive commands from ground", 200, NULL, 2, NULL);
	
	extern void state_manager (void *pvParameters);
	xTaskCreate(state_manager, "Task to handle all events", 100, NULL, 2, NULL);
	
	// Start Scheduler
	vTaskStartScheduler();

	/* Execution will only reach here if there was insufficient heap to start the scheduler. */
	for ( ;; );
	
	return 0;
}
