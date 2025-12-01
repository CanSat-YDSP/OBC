/*
 * uart.c
 *
 * Created: 19/10/2025 12:07:58 am
 *  Author: Yu Heng
 */ 

#include <stddef.h>
#include "FreeRTOS.h"
#include "uart.h"
#include "queue.h"
#include "portmacro.h"

QueueHandle_t uart1_rx_queue;

volatile uint8_t BNO_buffer[20];
volatile uint8_t buffer_index = 0;

void UART_init(uint16_t ubbr) {
	// for UART 0
	UBRR0H = (ubbr>>8); // set baud rate
	UBRR0L = (ubbr);
	UCSR0B = (1<<TXEN0) | (1<<RXEN0); // enable TX and RX
	UCSR0C = (1<<UCSZ00) | (1<<UCSZ01); // | (1<<USBS0); // 8 bit, 2 stop-bits
	
	// for UART 1
	UBRR1H = (ubbr>>8);
	UBRR1L = (ubbr);
	UCSR1B = (1<<TXEN1) | (1<<RXEN1) | (1<<RXCIE1);
	UCSR1C = (1<<UCSZ10) | (1<<UCSZ11);
}

void UART2_init() {
	uint16_t ubbr = 8; // taken from mega datasheet for 115200
	UBRR2H = (ubbr>>8);
	UBRR2L = ubbr;
	UCSR0B = (1<<TXEN2) | (1<<RXEN2);
	UCSR2C = (1<<UCSZ20) | (1<<UCSZ21);
}

void UART0_tx(uint8_t data) {
	while (!(UCSR0A & (1<<UDRE0)));
	UDR0 = data;
}

void UART1_tx(uint8_t data) {
	while (!(UCSR1A & (1<<UDRE1)));
	UDR1 = data;
}

void UART2_tx(uint8_t data) {
	while (!(UCSR2A & (1<<UDRE2)));
	UDR2 = data;
}

uint8_t UART0_rx() {
	while (!(UCSR0A & (1<<RXC0)));
	return UDR0;
}

// Deprecated, do not use
uint8_t UART1_rx() {
	while (!(UCSR1A & (1<<RXC1)));
	return UDR1;
}

uint8_t UART2_rx() {
	while (!(UCSR2A & (1<<RXC2)));
	return UDR2;
}

void print(char *s) {
	while (*s != '\0') {
		UART0_tx(*s);
		s++;
	}
}

void UART1_send_bytes(char *s, size_t size) {
	char* end = s + size;
	while (s < end) {
		UART1_tx(*s);
		s++;
	}
}

void UART2_send_bytes(char *s, size_t size) {
	char *end = s + size;
	while (s < end) {
		UART2_tx(*s);
		s++;
	}
}

// need find a way to drop command if no 0x0A
void UART1_receive_bytes(uint8_t *buf) {
	//if (UART1_rx() == 0xFF) {
		//int i = 0;
		//char c = UART1_rx();
		//UART0_send_bytes(c, 1);
		//while (c != 0x0A) {
			//UART0_send_bytes(&c, 1);
			//buf[i] = c;
			//c = UART1_rx();
			//UART0_send_bytes(&c, 1);
			//i++;
		//}
		//UART0_tx(0xFF);
		//buf[i] = '\0';
	//}
	uint8_t c;
	size_t i = 0;

	while (1) {
		if (xQueueReceive(uart1_rx_queue, &c, portMAX_DELAY) == pdTRUE) {
			if (c == 0xFF) {
				i = 0;
				do {
					xQueueReceive(uart1_rx_queue, &c, portMAX_DELAY);
					if (c != 0x0A) {
						buf[i++] = c; 
					}
				} while (c != 0x0A);
				
				buf[i] = '\0';
				break;
			}
		}
	}
}

ISR(USART1_RX_vect) {
	uint8_t c = UDR1;
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	xQueueSendFromISR(uart1_rx_queue, &c, &xHigherPriorityTaskWoken);
	
	if (xHigherPriorityTaskWoken) {
		portYIELD();
	}
}

ISR(USART2_RX_vect) {
	uint8_t c = UDR2;
	BNO_buffer[buffer_index++] = c;
}

// Deprecated
uint8_t UART1_is_ready() {
	return (UCSR1A & (1<<RXC1));
}

void UART0_send_bytes(char *s, size_t size) {
	char* end = s + size;
	while (s < end) {
		UART0_tx(*s);
		s++;
	}
}