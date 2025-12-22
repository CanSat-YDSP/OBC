/*
 * XBEE.c
 *
 * Created: 22.12.2025 09:59:52
 *  Author: ngyuh
 */ 

#include <util/delay.h>
#include "FreeRTOS.h"
#include "task.h"

#include "XBEE.h"
#include "uart.h"

#define AT_TIMEOUT_MS 2000  // maximum wait time for each AT command

uint8_t is_updating = 0;

void wait_for_response() {
	TickType_t start;
	uint8_t is_ready;
	
	start = xTaskGetTickCount();
	while(!(is_ready = UART1_receive_ok()) && (xTaskGetTickCount() - start) < pdMS_TO_TICKS(AT_TIMEOUT_MS)) {
		vTaskDelay(pdMS_TO_TICKS(10));
	}
	if(!is_ready) {
		print("XBEE did not respond.\r\n");
	}
}

void send_AT_command(uint8_t *buf, uint8_t len)
{
	is_updating = 1;
	
	// Enter AT mode
	vTaskDelay(pdMS_TO_TICKS(1100));
	UART1_send_bytes((uint8_t *)"+++", 3);
	wait_for_response();
	print("OK\r\n");

	// AT command
	UART1_send_bytes(buf, len);
	char cr = '\r';
	UART1_send_bytes((uint8_t *)&cr, 1);
	wait_for_response();
	print("OK\r\n");
	
	
	// save changes and leave command mode
	UART1_send_bytes((uint8_t *)"ATAC\r", 5);
	wait_for_response();
	print("OK\r\n");
	
	UART1_send_bytes((uint8_t *)"ATWR\r", 5);
	wait_for_response();
	print("OK\r\n");
	
	UART1_send_bytes((uint8_t *)"ATCN\r", 5);
	wait_for_response();
	print("OK\r\n");

	is_updating = 0;
	print("AT command sequence completed successfully!\r\n");
}