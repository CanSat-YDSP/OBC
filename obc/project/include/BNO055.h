/*
 * BNO055.h
 *
 * Created: 08.12.2025 15:16:30
 *  Author: Yu Heng
 */ 


#ifndef BNO055_H_
#define BNO055_H_

#define ACC_CAL 100
#define MAG_CAL 16
#define GYR_CAL 16

void BNO055_read(uint8_t addr, uint8_t *buf);
void BNO055_multiread(uint8_t addr, uint8_t *buf, size_t size);
void BNO055_write(uint8_t addr, uint8_t data);
void BNO055_init();
void BNO055_read_test(uint8_t *response);
uint8_t BNO055_test();

void BNO055_read_temp(int8_t *buf);
void BNO055_read_acc(float *acc);
void BNO055_read_mag(float *mag);
void BNO055_read_gyr(float *gyr);

#endif /* BNO055_H_ */