/*
 * i2c.c
 *
 * Created: 08.12.2025 13:57:32
 *  Author: Yu Heng
 */ 

#include <avr/io.h>

void i2c_init() {
	TWBR = 12; // 400kHz from 16MHz clock
	TWCR = (1 << TWEN);
}

void i2c_send_start() {
	TWCR = (1<<TWINT) | (1<<TWSTA) | (1 << TWEN);
	
	// wait for TWINT flag to be set
	while (!(TWCR & (1<<TWINT)));
	
	//uint8_t a = (TWSR & 0xF8);
	//char output[20];
	//sprintf(output, "START: %x\r\n", a);
	//print(output);
}

void i2c_send_stop() {
	TWCR = (1<<TWINT) | (1<<TWSTO) | (1<<TWEN);
}

void i2c_enter_MT_mode(uint8_t addr) {
	TWDR = (addr << 1);
	TWCR = (1<<TWINT) | (1<<TWEN);
	
	// wait for TWINT flag to be set
	while (!(TWCR & (1<<TWINT)));
	
	//uint8_t a = (TWSR & 0xF8);
	//char output[40];
	//sprintf(output, "Enter MT Mode: %x\r\n", a);
	//print(output);
}

void i2c_enter_MR_mode(uint8_t addr) {
	TWDR = (addr << 1) | 0x01;
	TWCR = (1<<TWINT) | (1<<TWEN);
	
	// wait for TWINT flag to be set
	while (!(TWCR & (1<<TWINT)));
	
	//uint8_t a = (TWSR & 0xF8);
	//char output[40];
	//sprintf(output, "Enter MR mode: %x\r\n", a);
	//print(output);
}

void i2c_send(uint8_t data) {
	TWDR = data;
	TWCR = (1<<TWINT) | (1<<TWEN);
	
	// wait for TWINT flag to be set
	while (!(TWCR & (1<<TWINT)));
}

void i2c_read_nack(uint8_t *buffer) {
	TWCR = (1<<TWINT) | (1<<TWEN);
	
	// wait for TWINT flag to be set
	while (!(TWCR & (1<<TWINT)));
	
	*buffer = TWDR;
}

void i2c_read_ack(uint8_t *buffer) {
	TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWEA);
	
	// wait for TWINT flag to be set
	while (!(TWCR & (1<<TWINT)));
	
	*buffer = TWDR;
}