/*
 * i2c.h
 *
 * Created: 08.12.2025 13:57:42
 *  Author: Yu Heng
 */ 


#ifndef I2C_H_
#define I2C_H_

void i2c_init();
void i2c_send_start();
void i2c_send_stop();
void i2c_enter_MT_mode(uint8_t addr);
void i2c_enter_MR_mode(uint8_t addr);
void i2c_send(uint8_t data);
void i2c_read_nack(uint8_t *buffer);
void i2c_read_ack(uint8_t *buffer);

#endif /* I2C_H_ */