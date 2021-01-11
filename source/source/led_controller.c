#include <avr/io.h>
#include <avr/interrupt.h>

#include "common.h"
#include "led_controller.h"

#define LED_A 0b00000100
#define LED_B 0b00000001
#define LED_C 0b01000000
#define LED_D 0b00001000
#define LED_E 0b00010000
#define LED_F 0b00000010
#define LED_G 0b00100000

uint8_t data_cache[4];
uint8_t select_flag = 0;

uint8_t power_flag = 0;

uint8_t appTimerFlag = 0;

//7-seg LED Port Assign
/*
A: PB2
B: PB0
C: PB6
D: PB3
E: PB4
F: PB1
G: PB5
*/

void set_port(uint8_t dec);

ISR(TIMER1_COMPA_vect)
{
	appTimerFlag = 1;
	
	PORTD &= 0b11000011;
	
	if (power_flag) {
		PORTB &= 0x80;
		return;
	} else {
		set_port(data_cache[select_flag]);
		select_flag++;
	}	
	
	if (select_flag == 1) {
		PORTD |= 0b00000100;
	} else if (select_flag == 2) {
		PORTD |= 0b00010000;
	} else if (select_flag == 3) {
		PORTD |= 0b00100000;
	} else {
		PORTD |= 0b00001000;
		select_flag = 0;
	}
}

void set_7seg_timer(void)
{
	TCCR1A = 0b00000000;
	TCCR1B = 0b00001011;
	OCR1A = 128;
	OCR1B = OCR1A / 2;
	
	TIMSK |= 0b01000000;
	
	sei();
}

void set_port(uint8_t dec)
{
	PORTB |= 0b01111111;
	if (dec == 0xFF) {
		//PORTB &= 0b10000000;
		return;
	}
	
	if (dec == 0) {
		//A-B-C-D-E-F
		PORTB &= ~(LED_A | LED_B | LED_C | LED_D | LED_E | LED_F);
	} else if (dec == 1) {	
		//B-C
		PORTB &= ~(LED_B | LED_C);
	} else if (dec == 2) {	
		//A-B-D-E-G
		PORTB &= ~(LED_A | LED_B | LED_D | LED_E | LED_G);
	} else if (dec == 3) {	
		//A-B-C-D-G
		PORTB &= ~(LED_A | LED_B | LED_C | LED_D | LED_G);
	} else if (dec == 4) {	
		//B-C-F-G
		PORTB &= ~(LED_B | LED_C | LED_F | LED_G);
	} else if (dec == 5) {	
		//A-C-D-F-G
		PORTB &= ~(LED_A | LED_C | LED_D | LED_F | LED_G);
	} else if (dec == 6) {	
		//A-C-D-E-F-G
		PORTB &= ~(LED_A | LED_C | LED_D | LED_E | LED_F | LED_G);
	} else if (dec == 7) {	
		//A-B-C
		PORTB &= ~(LED_A | LED_B | LED_C);
	} else if (dec == 8) {	
		//A-B-C-D-E-F-G
		PORTB &= ~(LED_A | LED_B | LED_C | LED_D | LED_E | LED_F | LED_G);
	} else if (dec == 9) {	
		//A-B-C-D-F-G
		PORTB &= ~(LED_A | LED_B | LED_C | LED_D | LED_F | LED_G);
	} else if (dec == 0x81) {
		//P (A-B-E-F-G)
		PORTB &= ~(LED_A | LED_B | LED_E | LED_F | LED_G);
	} else if (dec == 0x82) {
		//R (A-E-F)
		PORTB &= ~(LED_A | LED_E | LED_F);
	} else if (dec == 0x83) {
		//O (A-B-C-D-E-F)
		PORTB &= ~(LED_A| LED_B | LED_C | LED_D | LED_E | LED_F);
	} else if (dec == 0x84) {
		//G (A-B-C-D-F-G)
		PORTB &= ~(LED_A | LED_B | LED_C | LED_D | LED_F | LED_G);
	}
}

void set_7seg_data(uint16_t data)
{
	uint8_t i;
	
	for (i = 0; i < 4; i++) {
		if ((data == 0) && (i != 0)) {
			data_cache[i] = 0xFF;
		} else {
			data_cache[i] = data % 10;
			data = data / 10;
		}
	}
}

void set_led_power_stat(uint8_t stat)
{
	power_flag = stat;
}

void set_prog_mode(void)
{
	data_cache[0] = 0x84;
	data_cache[1] = 0x83;
	data_cache[2] = 0x82;
	data_cache[3] = 0x81;
}

uint8_t appTimer(void) {
	if (appTimerFlag) {
		appTimerFlag = 0;
		return (1);
	}
	return (0);
}