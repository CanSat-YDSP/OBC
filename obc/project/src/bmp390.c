/*
 * bmp390.c
 *
 * Created: 02.12.2025 13:18:32
 *  Author: Yu Heng
 */ 

#include "spi.h"
#include "bmp390.h"
#include <util/delay.h>

BMP390_trimming_coeff trimming_coeffs;
BMP390_calculated_coeff calculated_coeffs;
BMP390_raw_data raw_data;

uint8_t BMP390_readID() {
	SPI_select(SS_BMP390);
	SPI_transaction(0x00 | 0x80);
	SPI_transaction(0xFF);
	uint8_t data = SPI_transaction(0xFF);
	SPI_deselect(SS_BMP390);
	return data;
}

void BMP390_get_compensations() {
	SPI_select(SS_BMP390);
	SPI_transaction(0x31 | 0x80);
	SPI_transaction(0x00);
	uint8_t *trim_ptr = (uint8_t*)&trimming_coeffs;
	
	for (int i = 0; i < sizeof(BMP390_trimming_coeff); i++) {
		trim_ptr[i] = SPI_transaction(0x00);
	}
	
	SPI_deselect(SS_BMP390);
}

void BMP390_calculate_compensations() {
	BMP390_get_compensations(); // read raw NVM values

	// --- Temperature coefficients ---
	calculated_coeffs.PAR_T1 = trimming_coeffs.NVM_PAR_T1 * (256.0f);              // 2^-8
	calculated_coeffs.PAR_T2 = trimming_coeffs.NVM_PAR_T2 * (1.0f / 1073741824.0f);      // 2^30
	calculated_coeffs.PAR_T3 = trimming_coeffs.NVM_PAR_T3 * (1.0f / 281474976710656.0f); // 2^48

	// --- Pressure coefficients ---
	calculated_coeffs.PAR_P1  = ((float)trimming_coeffs.NVM_PAR_P1 - 16384.0f) / 1048576.0f;   // (P1 - 2^14)/2^20
	calculated_coeffs.PAR_P2  = ((float)trimming_coeffs.NVM_PAR_P2 - 16384.0f) / 536870912.0f; // (P2 - 2^14)/2^29
	calculated_coeffs.PAR_P3  = (float)trimming_coeffs.NVM_PAR_P3 * (1.0f / 4294967296.0f);    // 2^-32
	calculated_coeffs.PAR_P4  = (float)trimming_coeffs.NVM_PAR_P4 * (1.0f / 137438953472.0f);  // 2^-37
	calculated_coeffs.PAR_P5  = (float)(trimming_coeffs.NVM_PAR_P5 >> 3);                        // right shift 3, signed
	calculated_coeffs.PAR_P6  = (float)trimming_coeffs.NVM_PAR_P6 * (1.0f / 64.0f);             // 2^-6
	calculated_coeffs.PAR_P7  = (float)trimming_coeffs.NVM_PAR_P7 * (1.0f / 256.0f);            // 2^-8
	calculated_coeffs.PAR_P8  = (float)trimming_coeffs.NVM_PAR_P8 * (1.0f / 32768.0f);          // 2^-15
	calculated_coeffs.PAR_P9  = (float)trimming_coeffs.NVM_PAR_P9 * (1.0f / 281474976710656.0f); // 2^-48
	calculated_coeffs.PAR_P10 = (float)trimming_coeffs.NVM_PAR_P10 * (1.0f / 281474976710656.0f);// 2^-48
	calculated_coeffs.PAR_P11 = (float)trimming_coeffs.NVM_PAR_P11 * (1.0f / 3.6893488e19f);    // 2^-65
}

void BMP390_init() {
	
	// set osr
	SPI_select(SS_BMP390);
	SPI_transaction(0x1C & 0x7F);
	SPI_transaction(0x03);
	SPI_deselect(SS_BMP390);
	
	// set odr
	SPI_select(SS_BMP390);
	SPI_transaction(0x1D & 0x7F);
	SPI_transaction(0x02);
	SPI_deselect(SS_BMP390);
	
	// enter normal mode
	
	SPI_select(SS_BMP390);
	SPI_transaction(0x1B & 0x7F);
	SPI_transaction(0x33);
	SPI_deselect(SS_BMP390);
	
	BMP390_calculate_compensations();
	
	//SPI_select(SS_BMP390);
	//SPI_transaction(0x1C & 0x7F);
	//SPI_transaction(0x1B);
	//SPI_deselect(SS_BMP390);
	
	raw_data.temperature = 0;
	raw_data.pressure = 0;
	_delay_ms(2000);
}

void BMP390_read_PWR() {
	SPI_select(SS_BMP390);
	SPI_transaction(0x1B | 0x80);
	SPI_transaction(0xFF);
	uint8_t response = SPI_transaction(0xFF);
	SPI_deselect(SS_BMP390);
	UART0_send_bytes(&response, 1);
}

void BMP390_check_status() {
	SPI_select(SS_BMP390);
	SPI_transaction(0x02 | 0x80);
	SPI_transaction(0xFF);
	uint8_t response = SPI_transaction(0xFF);
	SPI_deselect(SS_BMP390);
	UART0_send_bytes(&response, 1);
}

void BMP390_burst_read() {
	uint8_t data[6] = {0};	
	
	SPI_select(SS_BMP390);
	SPI_transaction(0x04 | 0x80);
	SPI_transaction(0x00); // dummy byte
	for (int i = 0; i < 6; i++) {
		data[i] = SPI_transaction(0x00);
	}
	raw_data.temperature = (data[0]) | (data[1] << 8) | (data[2] << 16);
	if (raw_data.temperature & 0x800000) {
		raw_data.temperature |= 0xFF000000;
	}
	
	raw_data.pressure = (data[3]) | (data[4] << 8) | (data[5] << 16);
	
	SPI_deselect(SS_BMP390);
	
	// UART0_send_bytes((uint8_t*)(&raw_data.temperature), 3);
}

void BMP390_get_temp_data(float *temperature) {
	float partial_data1;
	float partial_data2;
	
	// UART0_send_bytes(&raw_data.temperature, 3);
	
	partial_data1 = (float)(raw_data.temperature - calculated_coeffs.PAR_T1);
	partial_data2 = (float)(partial_data1 * calculated_coeffs.PAR_T2);
	
	*temperature = partial_data2 + (partial_data1 * partial_data1) * calculated_coeffs.PAR_T3;
}