/*
 * XBEE.h
 *
 * Created: 22.12.2025 10:00:13
 *  Author: ngyuh
 */ 

#include <avr/io.h>

#ifndef XBEE_H_
#define XBEE_H_

extern uint8_t is_updating;

void send_AT_command(uint8_t *buf, uint8_t len);

#endif /* XBEE_H_ */