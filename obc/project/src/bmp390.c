/*
 * BMP390.c
 *
 * Created: 04.12.2025 14:41:31
 *  Author: ngyuh
 */ 
#include <stddef.h>
#include "spi.h"
#include "BMP390.h"

BMP390_raw_data raw_readings;
BMP390_trimming_coeff raw_calibs;
BMP390_calculated_coeff calculated_calibs;

void BMP390_read_series(uint8_t addr, uint8_t *response, size_t len) {
	SPI_select(SS_BMP390);
	SPI_transaction(addr | READ); // send read command
	SPI_transaction(0x00); // send extra dummy byte
	
	for (int i = 0; i < len; i++) {
		response[i] = SPI_transaction(0x00);
	}
	
	SPI_deselect(SS_BMP390);
}

void BMP390_read(uint8_t addr, uint8_t *response) {
	SPI_select(SS_BMP390);
	SPI_transaction(addr | READ); // send read command
	SPI_transaction(0x00); // send extra dummy byte
	*response = SPI_transaction(0x00);
	SPI_deselect(SS_BMP390);
}

void BMP390_write(uint8_t addr, uint8_t write_byte) {
	SPI_select(SS_BMP390);
	SPI_transaction(addr & WRITE);
	SPI_transaction(write_byte);
	SPI_deselect(SS_BMP390);
}

void BMP390_init() {
	BMP390_write(0x1C, 0x03); // set osr
	BMP390_write(0x1D, 0x02); // set odr
	BMP390_write(0x1B, 0x33); // enter normal mode
	
	BMP390_read_calib(); // read and calculated calibrations
}

uint8_t BMP390_test() {
	uint8_t id;
	BMP390_read_id(&id);
	return (id == 0x60);
}

void BMP390_read_id(uint8_t *response) {
	BMP390_read(CHIP_ID, response);
}

void BMP390_read_status(uint8_t *response) {
	BMP390_read(STATUS, response);
}

void BMP390_read_data(uint32_t *pressure, uint32_t *temperature) {
	uint8_t data_bytes[6] = {0};
	BMP390_read_series(DATA, data_bytes, 6);
	
	*pressure = CONCAT_3_BYTES(data_bytes[2], data_bytes[1], data_bytes[0]);
	*temperature = CONCAT_3_BYTES(data_bytes[5], data_bytes[4], data_bytes[3]);
}

void BMP390_read_calib() {
	// create reg_data_array
	uint8_t reg_data[21] = {0};
	BMP390_read_series(CALIB_DATA, reg_data, 21);
	parse_calib_data(reg_data);
}

void BMP390_get_readings(float *pressure, float *temperature) {
	BMP390_read_data(&raw_readings.pressure, &raw_readings.temperature); // get raw data values
	
	BMP390_get_pres_data(pressure, BMP390_get_temp_data(temperature));
}

void BMP390_get_temp_data(float *temperature) {
	double partial_data1;
	double partial_data2;
	
	// UART0_send_bytes(&raw_data.temperature, 4);
	
	partial_data1 = (double)(raw_readings.temperature - calculated_calibs.par_t1);
	partial_data2 = (double)(partial_data1 * calculated_calibs.par_t2);
	
	*temperature = partial_data2 + (partial_data1 * partial_data1) * calculated_calibs.par_t3;
}

void BMP390_get_pres_data(float *pressure, float calculated_temperature)
{

	/* Variable to store the compensated pressure */
	double comp_press;

	/* Temporary variables used for compensation */
	double partial_data1;
	double partial_data2;
	double partial_data3;
	double partial_data4;
	double partial_out1;
	double partial_out2;

	partial_data1 = calculated_calibs.par_p6 * calculated_temperature;
	partial_data2 = calculated_calibs.par_p7 * (calculated_temperature * calculated_temperature);
	partial_data3 = calculated_calibs.par_p8 * (calculated_temperature * calculated_temperature * calculated_temperature);
	partial_out1 = calculated_calibs.par_p5 + partial_data1 + partial_data2 + partial_data3;
	
	partial_data1 = calculated_calibs.par_p2 * calculated_temperature;
	partial_data2 = calculated_calibs.par_p3 * (calculated_temperature * calculated_temperature);
	partial_data3 = calculated_calibs.par_p4 * (calculated_temperature * calculated_temperature * calculated_temperature);
	partial_out2 = raw_readings.pressure *
	(calculated_calibs.par_p1 + partial_data1 + partial_data2 + partial_data3);
	
	partial_data1 = (raw_readings.pressure * raw_readings.pressure);
	partial_data2 = calculated_calibs.par_p9 + calculated_calibs.par_p10 * calculated_temperature;
	partial_data3 = partial_data1 * partial_data2;
	partial_data4 = partial_data3 + (raw_readings.pressure * raw_readings.pressure * raw_readings.pressure) * calculated_calibs.par_p11;
	comp_press = partial_out1 + partial_out2 + partial_data4;

	*pressure = comp_press;
}

void parse_calib_data(const uint8_t *reg_data)
{
	/* Temporary variable */
	double temp_var;

	/* 1 / 2^8 */
	temp_var = 0.00390625f;
	raw_calibs.par_t1 = CONCAT_BYTES(reg_data[1], reg_data[0]);
	calculated_calibs.par_t1 = ((double)raw_calibs.par_t1 / temp_var);
	raw_calibs.par_t2 = CONCAT_BYTES(reg_data[3], reg_data[2]);
	temp_var = 1073741824.0f;
	calculated_calibs.par_t2 = ((double)raw_calibs.par_t2 / temp_var);
	raw_calibs.par_t3 = (int8_t)reg_data[4];
	temp_var = 281474976710656.0f;
	calculated_calibs.par_t3 = ((double)raw_calibs.par_t3 / temp_var);
	raw_calibs.par_p1 = (int16_t)CONCAT_BYTES(reg_data[6], reg_data[5]);
	temp_var = 1048576.0f;
	calculated_calibs.par_p1 = ((double)(raw_calibs.par_p1 - (16384)) / temp_var);
	raw_calibs.par_p2 = (int16_t)CONCAT_BYTES(reg_data[8], reg_data[7]);
	temp_var = 536870912.0f;
	calculated_calibs.par_p2 = ((double)(raw_calibs.par_p2 - (16384)) / temp_var);
	raw_calibs.par_p3 = (int8_t)reg_data[9];
	temp_var = 4294967296.0f;
	calculated_calibs.par_p3 = ((double)raw_calibs.par_p3 / temp_var);
	raw_calibs.par_p4 = (int8_t)reg_data[10];
	temp_var = 137438953472.0f;
	calculated_calibs.par_p4 = ((double)raw_calibs.par_p4 / temp_var);
	raw_calibs.par_p5 = CONCAT_BYTES(reg_data[12], reg_data[11]);

	/* 1 / 2^3 */
	temp_var = 0.125f;
	calculated_calibs.par_p5 = ((double)raw_calibs.par_p5 / temp_var);
	raw_calibs.par_p6 = CONCAT_BYTES(reg_data[14], reg_data[13]);
	temp_var = 64.0f;
	calculated_calibs.par_p6 = ((double)raw_calibs.par_p6 / temp_var);
	raw_calibs.par_p7 = (int8_t)reg_data[15];
	temp_var = 256.0f;
	calculated_calibs.par_p7 = ((double)raw_calibs.par_p7 / temp_var);
	raw_calibs.par_p8 = (int8_t)reg_data[16];
	temp_var = 32768.0f;
	calculated_calibs.par_p8 = ((double)raw_calibs.par_p8 / temp_var);
	raw_calibs.par_p9 = (int16_t)CONCAT_BYTES(reg_data[18], reg_data[17]);
	temp_var = 281474976710656.0f;
	calculated_calibs.par_p9 = ((double)raw_calibs.par_p9 / temp_var);
	raw_calibs.par_p10 = (int8_t)reg_data[19];
	temp_var = 281474976710656.0f;
	calculated_calibs.par_p10 = ((double)raw_calibs.par_p10 / temp_var);
	raw_calibs.par_p11 = (int8_t)reg_data[20];
	temp_var = 36893488147419103232.0f;
	calculated_calibs.par_p11 = ((double)raw_calibs.par_p11 / temp_var);
}