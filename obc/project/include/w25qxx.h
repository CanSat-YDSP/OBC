/*
 * w25qxx.h
 *
 * Created: 20/10/2025 1:07:01 pm
 *  Author: Yu Heng
 */ 


#ifndef W25QXX_H_
#define W25QXX_H_

#include <avr/io.h>

void W25QXX_readID(uint8_t* mfr, uint8_t* memtype, uint8_t* cap);
uint8_t W25QXX_test();

#endif /* W25QXX_H_ */