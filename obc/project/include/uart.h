/*
 * uart.h
 *
 * Created: 19/10/2025 12:07:46 am
 *  Author: Yu Heng
 */ 


#ifndef UART_H_
#define UART_H_

#include <avr/io.h>
#include <stddef.h>
#include <FreeRTOS.h>
#include <queue.h>

#define RX0 PE0
#define TX0 PE1

extern QueueHandle_t uart1_rx_queue;
volatile uint8_t BNO_buffer[20];
extern volatile uint8_t buffer_index;

void UART_init(uint16_t ubbr);
void UART0_tx(uint8_t data);
uint8_t UART0_rx();
void UART1_tx(uint8_t data);
uint8_t UART1_rx();

void print(char* s);
void UART0_send_bytes(char *s, size_t size);

void UART1_send_bytes(char *s, size_t size);
void UART1_receive_bytes(uint8_t *buf);

uint8_t UART1_is_ready();

void UART2_send_bytes(char *s, size_t size);

#endif /* UART_H_ */