/*
 * bme390.h
 *
 * Created: 02.12.2025 13:18:57
 *  Author: ngyuh
 */ 


#ifndef BMP390_H_
#define BMP390_H_

typedef struct __attribute__((packed)) {
	uint16_t NVM_PAR_T1;
	uint16_t NVM_PAR_T2;
	int8_t NVM_PAR_T3;
	int16_t NVM_PAR_P1;
	int16_t NVM_PAR_P2;
	int8_t NVM_PAR_P3;
	int8_t NVM_PAR_P4;
	uint16_t NVM_PAR_P5;
	uint16_t NVM_PAR_P6;
	int8_t NVM_PAR_P7;
	int8_t NVM_PAR_P8;
	int16_t NVM_PAR_P9;
	int8_t NVM_PAR_P10;
	int8_t NVM_PAR_P11;
} BMP390_trimming_coeff;

typedef struct {
	// Temperature coefficients
	float PAR_T1;
	float PAR_T2;
	float PAR_T3;

	// Pressure coefficients
	float PAR_P1;
	float PAR_P2;
	float PAR_P3;
	float PAR_P4;
	float PAR_P5;
	float PAR_P6;
	float PAR_P7;
	float PAR_P8;
	float PAR_P9;
	float PAR_P10;
	float PAR_P11;
} BMP390_calculated_coeff;

typedef struct {
	int32_t pressure;
	int32_t temperature;
} BMP390_raw_data;

extern BMP390_calculated_coeff calculated_coeffs;
extern BMP390_trimming_coeff trimming_coeffs;
extern BMP390_raw_data raw_data;

#endif /* BMP390_H_ */