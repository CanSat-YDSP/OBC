/*
 * uart.h
 *
 * Created: 19/10/2025 12:07:46 am
 *  Author: Yu Heng
 */ 


#ifndef UART_H_
#define UART_H_

#include <avr/io.h>

#define RX0 PE0
#define TX0 PE1

void UART_init(uint16_t ubbr);
void UART0_tx(uint8_t data);
uint8_t UART0_rx();
void UART1_tx(uint8_t data);
uint8_t UART1_rx();

void print(char* s);
void UART1_print(char* s);

#endif /* UART_H_ */