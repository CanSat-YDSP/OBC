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

// 
#include "uart.h"
#include "spi.h"
#include "w25qxx.h"
#include "telemetry.h"
#include "states.h"
#include "servo.h"
#include "bmp390.h"

#define RX_QUEUE_LEN 30
#define EVENTS_QUEUE_LEN 10
#define SIM_PRESSURE_LEN 10

int main(void)
{
	sei();
	
	UART_init(UBBR);
	SPI_init();
	BMP390_init(); // get required coefficients for BMP390
	_delay_ms(1000);
	//BMP390_read_PWR();
	
	//move_servo();
	
	//_delay_ms(1000);
	
	//W25QXX_test();
	
	uint8_t id = BMP390_readID();
	UART0_send_bytes(&id, 1);
	print("\r\n--------\r\n");
	
	char buffer[50];
	while (1) {
		// BMP390_check_status();
		
		// BMP390_read_PWR();
		 //
		 BMP390_burst_read();
		 //
		 //BMP390_read_PWR();
		 // print("\r\n");
		 float temp;
		 BMP390_get_temp_data(&temp);
		//temp = 42.12;
		//print("Got Data!\r\n");
		
		// cheating here
		temp = -temp;
		temp -= 100.0f;
		
		 int wholePart = (int)temp;
		 int decimalPart = (int)((temp - wholePart) * 100); // For two decimal places

		sprintf(buffer, "Temperature: %d.%02d\r\n", wholePart, decimalPart);
		 print(buffer);
		_delay_ms(1000);
	}
	
	
	//clear_chip(); // always remember to clear memory before writing
	//uint32_t addr = 0x001000;
	//
	//uint8_t bytes[4] = {0xDE, 0xAD, 0xBE, 0xEF};
	//
	//write_page(addr, bytes, 4);
	//
	//uint8_t response;
	//for (int i = 0; i < 4; i++) {
		//response = read_data(addr + i);
		//UART0_send_bytes(&response, 1);
	//}
	
	
	//print("CatSat\r\n");
	//print("----------------\r\n");
	//print("Initializing...\r\n");
	//
	//stateMutex = xSemaphoreCreateMutex();
	//uart1_rx_queue = xQueueCreate(RX_QUEUE_LEN, sizeof(uint8_t));
	//events_queue = xQueueCreate(EVENTS_QUEUE_LEN, sizeof(CanSatEvents_t));
	//simulated_pressure_queue = xQueueCreate(SIM_PRESSURE_LEN, sizeof(float));
	//
	//extern void send_to_ground (void *pvParameters);
	//xTaskCreate(send_to_ground, "Task to send telemetry to ground", 200, NULL, 2, NULL);
	//
	//extern void receive_from_ground (void *pvParameters);
	//xTaskCreate(receive_from_ground, "Task to receive commands from ground", 200, NULL, 2, NULL);
	//
	//extern void state_manager (void *pvParameters);
	//xTaskCreate(state_manager, "Task to handle all events", 100, NULL, 2, NULL);
	//
	//print("Starting...\r\n");
	//
	//// Start Scheduler
	//vTaskStartScheduler();
	//
	///* Execution will only reach here if there was insufficient heap to start the scheduler. */
	// for ( ;; );
	
	return 0;
}
