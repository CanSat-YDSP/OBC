/*
 * BNO055.c
 *
 * Created: 08.12.2025 15:16:21
 *  Author: Yu Heng
 */ 

#include <avr/io.h>
#include "i2c.h"
#include "uart.h"
#include "bno055.h"
#include <stddef.h>

#define BNO055_ADDR 0x29

void BNO055_read(uint8_t addr, uint8_t *buf) {
	i2c_send_start();
	i2c_enter_MT_mode(BNO055_ADDR);
	i2c_send(addr);
	i2c_send_start();
	i2c_enter_MR_mode(BNO055_ADDR);
	i2c_read_nack(buf);
	i2c_send_stop();
}

void BNO055_multiread(uint8_t addr, uint8_t *buf, size_t size) {
	i2c_send_start();
	i2c_enter_MT_mode(BNO055_ADDR);
	i2c_send(addr);
	i2c_send_start();
	i2c_enter_MR_mode(BNO055_ADDR);
	for (int i = 0; i < size - 1; i++) {
		i2c_read_ack(&(buf[i]));
	}
	i2c_read_nack(&(buf[size-1]));
	i2c_send_stop();
}

void BNO055_write(uint8_t addr, uint8_t data) {
	i2c_send_start();
	i2c_enter_MT_mode(BNO055_ADDR);
	i2c_send(addr);
	i2c_send(data);
	i2c_send_stop();
}

void BNO055_init() {
	BNO055_write(0x3D, 0x0C); // enter NDOF mode
}

void BNO055_read_test(uint8_t *response) {
	BNO055_multiread(0x36, response, 1);
	//char output[10];
	//sprintf(output, "ID: %x\r\n", *response);
	//print(output);
}

uint8_t BNO055_test() {
	uint8_t id;
	BNO055_read_test(&id);
	return ((id & 0x0F) == 0x0F);
}

void BNO055_read_temp(int8_t *buf) {
	BNO055_multiread(0x34, buf, 1);
}

void BNO055_read_acc(float *accel) {
	uint8_t data_buf[6];
	BNO055_multiread(0x08, data_buf, 6);
	
	int16_t partial_x, partial_y, partial_z;
	
	partial_x = (int16_t)((data_buf[1] << 8) | data_buf[0]);
	partial_y = (int16_t)((data_buf[3] << 8) | data_buf[2]);
	partial_z = (int16_t)((data_buf[5] << 8) | data_buf[4]);
	
	accel[0] = (float)partial_x/ACC_CAL;
	accel[1] = (float)partial_y/ACC_CAL;
	accel[2] = (float)partial_z/ACC_CAL;
	
	//char buf[30];
	//sprintf(buf, "check acc_z: %d\r\n", (int)(accel[2]*100));
	//print(buf);
}

void BNO055_read_mag(float *mag) {
	uint8_t data_buf[6];
	BNO055_multiread(0x0E, data_buf, 6);
	
	int16_t partial_x, partial_y, partial_z;
	
	partial_x = (int16_t)((data_buf[1] << 8) | data_buf[0]);
	partial_y = (int16_t)((data_buf[3] << 8) | data_buf[2]);
	partial_z = (int16_t)((data_buf[5] << 8) | data_buf[4]);
	
	mag[0] = (float)partial_x/MAG_CAL;
	mag[1] = (float)partial_y/MAG_CAL;
	mag[2] = (float)partial_z/MAG_CAL;
}

void BNO055_read_gyr(float *gyr) {
	uint8_t data_buf[6];
	BNO055_multiread(0x14, data_buf, 6);
	
	int16_t partial_x, partial_y, partial_z;
	
	partial_x = (int16_t)((data_buf[1] << 8) | data_buf[0]);
	partial_y = (int16_t)((data_buf[3] << 8) | data_buf[2]);
	partial_z = (int16_t)((data_buf[5] << 8) | data_buf[4]);
	
	gyr[0] = (float)partial_x/GYR_CAL;
	gyr[1] = (float)partial_y/GYR_CAL;
	gyr[2] = (float)partial_z/GYR_CAL;
}