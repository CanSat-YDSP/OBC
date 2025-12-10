/*
 * BMP390.h
 *
 * Created: 04.12.2025 14:41:17
 *  Author: Yu Heng
 */ 


#ifndef BMP390_H_
#define BMP390_H_

#define CONCAT_BYTES(msb, lsb)			(((uint16_t)msb << 8) | (uint16_t)lsb)
#define CONCAT_3_BYTES(msb, lsb, xlsb)	(((uint32_t)msb << 16) | ((uint32_t)lsb << 8) | (uint32_t)xlsb)

#define CHIP_ID 0x00
#define STATUS 0x03
#define DATA 0x04
#define CALIB_DATA 0x31

#define READ 0x80
#define WRITE 0x7F

typedef struct {
	uint32_t pressure;
	uint32_t temperature;
} BMP390_raw_data;

typedef struct
{
    uint16_t par_t1;
    uint16_t par_t2;
    int8_t par_t3;
	
    int16_t par_p1;
    int16_t par_p2;
    int8_t par_p3;
    int8_t par_p4;
    uint16_t par_p5;
    uint16_t par_p6;
    int8_t par_p7;
    int8_t par_p8;
    int16_t par_p9;
    int8_t par_p10;
    int8_t par_p11;
} BMP390_trimming_coeff;

typedef struct {
	// Temperature coefficients
	double par_t1;
	double par_t2;
	double par_t3;

	// Pressure coefficients
	double par_p1;
	double par_p2;
	double par_p3;
	double par_p4;
	double par_p5;
	double par_p6;
	double par_p7;
	double par_p8;
	double par_p9;
	double par_p10;
	double par_p11;
} BMP390_calculated_coeff;

uint8_t BMP390_test(void);


#endif /* BMP390_H_ */