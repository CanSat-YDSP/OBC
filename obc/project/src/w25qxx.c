/*
 * W25Qxx.c
 *
 * Created: 20/10/2025 1:07:14 pm
 *  Author: Yu Heng
 */ 

#include "w25qxx.h"
#include "spi.h"
#include "uart.h"

void W25QXX_readID(uint8_t* mfr, uint8_t* memtype, uint8_t* cap) {
	SPI_select(SS_W25Qxx);
	SPI_transaction(0x9F);
	*mfr = SPI_transaction(0x00);
	*memtype = SPI_transaction(0x00);
	*cap = SPI_transaction(0x00);
	SPI_deselect(SS_W25Qxx);
}

uint8_t W25QXX_test() {
	uint8_t mfr, memtype, cap;
	W25QXX_readID(&mfr, &memtype, &cap);
	//char buffer[30];
	//sprintf(buffer, "JEDEC ID: %x %x %x", mfr, memtype, cap);
	//print(buffer);
	
	return (mfr == 0xEF);
}

uint8_t read_data(uint32_t addr) {
	SPI_select(SS_W25Qxx);
	SPI_transaction(0x03);
	SPI_transaction((addr>>16) & 0xFF);
	SPI_transaction((addr>>8) & 0xFF);
	SPI_transaction((addr) & 0xFF);
	uint8_t data = SPI_transaction(0x00);
	SPI_deselect(SS_W25Qxx);
	return data;
}

void release_power_down() {
	SPI_select(SS_W25Qxx);
	SPI_transaction(0xAB);
	SPI_deselect(SS_W25Qxx);
}

uint8_t read_status() {
	uint8_t byte;
	SPI_select(SS_W25Qxx);
	SPI_transaction(0x05);
	byte = SPI_transaction(0x00);
	//UART0_send_bytes(&byte, 1);
	SPI_deselect(SS_W25Qxx);
	return byte;
}

void write_enable() {
	SPI_select(SS_W25Qxx);
	SPI_transaction(0x06);
	SPI_deselect(SS_W25Qxx);
	while (!(read_status() & 0x02)); // bit1 = WEL
}

// max 256 bytes!
void write_page(uint32_t addr, uint8_t *bytes, size_t len) {
	write_enable();
	
	SPI_select(SS_W25Qxx);
	SPI_transaction(0x02);
	SPI_transaction((addr>>16) & 0xFF);
	SPI_transaction((addr>>8) & 0xFF);
	SPI_transaction((addr) & 0xFF);
	
	for (int i = 0; i < len; i++) {
		SPI_transaction(bytes[i]);
	}
	
	SPI_deselect(SS_W25Qxx);
	
	while (read_status() & 0x01);
}

void clear_chip() {
	write_enable();
	SPI_select(SS_W25Qxx);
	SPI_transaction(0x60);
	SPI_deselect(SS_W25Qxx);
	
	while (read_status() & 0x01);
}