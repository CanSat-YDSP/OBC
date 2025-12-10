/*
 * W25Qxx.h
 *
 * Created: 20/10/2025 1:07:01 pm
 *  Author: Yu Heng
 */ 


#ifndef W25QXX_H_
#define W25QXX_H_

#include <avr/io.h>
#include <stddef.h>

void W25QXX_readID(uint8_t* mfr, uint8_t* memtype, uint8_t* cap);
uint8_t W25QXX_test();

uint8_t read_data(uint32_t addr);
void release_power_down();
uint8_t read_status();
void write_page(uint32_t addr, uint8_t *bytes, size_t len);
void clear_chip();

#endif /* W25QXX_H_ */