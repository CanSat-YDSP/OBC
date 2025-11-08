/*
 * uart.c
 *
 * Created: 19/10/2025 12:07:58 am
 *  Author: Yu Heng
 */ 

#include <stddef.h>
#include "uart.h"

void UART_init(uint16_t ubbr) {
	
	// for UART 0
	UBRR0H = (ubbr>>8); // set baud rate
	UBRR0L = (ubbr);
	UCSR0B = (1<<TXEN0) | (1<<RXEN0); // enable TX and RX
	UCSR0C = (1<<UCSZ00) | (1<<UCSZ01); // | (1<<USBS0); // 8 bit, 2 stop-bits
	
	// for UART 1
	UBRR1H = (ubbr>>8);
	UBRR1L = (ubbr);
	UCSR1B = (1<<TXEN1) | (1<<RXEN1);
	UCSR1C = (1<<UCSZ10) | (1<<UCSZ11);
}

void UART0_tx(uint8_t data) {
	while (!(UCSR0A & (1<<UDRE0)));
	UDR0 = data;
}

void UART1_tx(uint8_t data) {
	while (!(UCSR1A & (1<<UDRE1)));
	UDR1 = data;
	
}

uint8_t UART0_rx() {
	while (!(UCSR0A & (1<<RXC0)));
	return UDR0;
}

uint8_t UART1_rx() {
	while (!(UCSR1A & (1<<RXC1)));
	return UDR1;
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

void UART1_receive_bytes(char *s) {
	
}