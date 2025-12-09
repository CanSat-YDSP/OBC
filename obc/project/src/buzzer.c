/*
 * buzzer.c
 *
 * Created: 08.12.2025 12:46:31
 *  Author: Yu Heng
 */ 

#include <avr/io.h>

void buzzer_start() {
	DDRL |= (1<<PL5);
	PORTL |= (1<<PL5);
}

void buzzer_stop() {
	DDRL &= ~(1<<PL5);
	PORTL &= ~(1<<PL5);
}