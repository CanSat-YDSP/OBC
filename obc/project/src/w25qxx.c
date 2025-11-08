/*
 * w25qxx.c
 *
 * Created: 20/10/2025 1:07:14 pm
 *  Author: Yu Heng
 */ 

#include "w25qxx.h"
#include "spi.h"
#include "uart.h"

void W25QXX_readID(uint8_t* mfr, uint8_t* memtype, uint8_t* cap) {
	SPI_select(SS);
	SPI_transaction(0x9F);
	*mfr = SPI_transaction(0x00);
	*memtype = SPI_transaction(0x00);
	*cap = SPI_transaction(0x00);
	SPI_deselect(SS);
}

uint8_t W25QXX_test() {
	uint8_t mfr, memtype, cap;
	W25QXX_readID(&mfr, &memtype, &cap);
	char buffer[30];
	sprintf(buffer, "JEDEC ID: %x %x %x", mfr, memtype, cap);
	//print(buffer);
	
	return (mfr == 0xEF);
}