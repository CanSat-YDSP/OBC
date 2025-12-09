/*
 * servo.c
 *
 * Created: 02.12.2025 11:00:09
 *  Author: Yu Heng
 */

#include <avr/io.h>

void move_servo() {
	DDRL |= (1<<PL4);
	
	// refer to notes for details, PIN 45
	TCCR5A = 0b00100010;
	TCCR5B = 0b00011011;
	ICR5 = 4999;
	OCR5B = 250; // 2ms, to the right
}

void reset_servo() {
	DDRL |= (1<<PL4);
	
	// refer to notes for details, PIN 45
	TCCR5A = 0b00100010;
	TCCR5B = 0b00011011;
	ICR5 = 4999;
	OCR5B = 500; // 2ms, to the right
}

void stop_servo() {
	DDRL &= ~(1<<PL4);
}
