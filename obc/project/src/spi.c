/*
 * spi.c
 *
 * Created: 18/10/2025 11:02:46 pm
 *  Author: Yu Heng
 */ 

#include "spi.h"

void SPI_init(void) {
	DDR_SPI = (1<<SS) | (1<<SCK) | (1<<MOSI); // set relevant pins to output
	SPCR = (1<<SPE) | (1<<MSTR) | (1<<SPR0); // enable spi, set as master, use mode 0, set sck = f/16
	PORTB |= (1<<SS); // add when more slaves added
}

void SPI_select(uint8_t addr) {
	PORTB &= ~(1<<addr);
}

void SPI_deselect(uint8_t addr) {
	PORTB |= (1<<addr);
}

uint8_t SPI_transaction(uint8_t data) {
	SPDR = data;
	while (!(SPSR & (1<<SPIF))); // wait until SPIF is 1
	return SPDR;
}