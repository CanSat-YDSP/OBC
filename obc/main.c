#define F_CPU 16000000L // Specify oscillator frequency
#define BAUD_RATE 9600
#define UBBR ((F_CPU)/(16UL*BAUD_RATE) - 1) // for UART_init

#include <tasks.h>

// FreeRTOS
#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include <semphr.h>
#include <util/delay.h>
#include <avr/eeprom.h>

// 
#include "uart.h"
#include "spi.h"
#include "w25qxx.h"
#include "telemetry.h"
#include "states.h"
#include "servo.h"
#include "BMP390.h"
#include "BNO055.h"

#define RX_QUEUE_LEN 30
#define EVENTS_QUEUE_LEN 10
#define SIM_PRESSURE_LEN 10

void component_init_and_tests() {
	
	sei();
	
	SPI_init();
	i2c_init();
	BMP390_init(); // get required coefficients for BMP390
	BNO055_init();
	// W25QXX_init(); // Only use in BOOTLOADER
	
	print("CatSat 75percent\r\n");
	print("----------------\r\n");
	print("Initializing and Testing...\r\n");
	
	// Component Tests
	char output[30];
	// W25Q32
	sprintf(output, "W25Q32: %d\r\n", W25QXX_test());
	print(output);
	// BMP390
	sprintf(output, "BMP390: %d\r\n", BMP390_test());
	print(output);
	// BNO055
	sprintf(output, "BNO055: %d\r\n", BNO055_test());
	print(output);
	// servo and buzzer
	//move_servo();
	buzzer_start();
	_delay_ms(500);
	buzzer_stop();
	reset_servo();
	_delay_ms(500);
}

int main(void)
{	
	// read app size from the bootloader
	
	UART_init(UBBR);
	
	uint32_t app_size = eeprom_read_dword((uint32_t*)0x00);
	
	uint8_t size_buf[20];
	sprintf(size_buf, "size: %lu\r\n", app_size);
	print(size_buf);
	
	component_init_and_tests();
	
	stateMutex = xSemaphoreCreateMutex();
	uart1_rx_queue = xQueueCreate(RX_QUEUE_LEN, sizeof(uint8_t));
	events_queue = xQueueCreate(EVENTS_QUEUE_LEN, sizeof(CanSatEvents_t));
	simulated_pressure_queue = xQueueCreate(SIM_PRESSURE_LEN, sizeof(float));
	
	extern void send_to_ground (void *pvParameters);
	xTaskCreate(send_to_ground, "Task to send telemetry to ground", 200, NULL, 2, NULL);
	
	extern void receive_from_ground (void *pvParameters);
	xTaskCreate(receive_from_ground, "Task to receive commands from ground", 500, NULL, 2, NULL);
	
	extern void state_manager (void *pvParameters);
	xTaskCreate(state_manager, "Task to handle all events", 100, NULL, 2, NULL);
	
	extern void data_reading (void *pvParameters);
	xTaskCreate(data_reading, "Task to read data from sensors", 400, NULL, 2, NULL);
	
	extern void pgm_verifier(void *pvParameters);
	xTaskCreate(pgm_verifier, "Task to calculate checksum of the app", 100, (void *)&app_size, 2, NULL);
	
	extern void bootWatchdog(void *pvParameters);
	xTaskCreate(bootWatchdog, "Task to enter bootloader if no response", 100, NULL, 2, NULL);
	
	print("Starting...\r\n");
	
	// Start Scheduler
	vTaskStartScheduler();
	
	/* Execution will only reach here if there was insufficient heap to start the scheduler. */
	 for ( ;; );
	
	return 0;
}
