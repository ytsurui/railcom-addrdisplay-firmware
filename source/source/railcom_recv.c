/*
 * railcom_recv.c
 *
 * Created: 2020/12/29 0:27:55
 *  Author: ytsurui
 */ 

#include <avr/io.h>
#include "led_controller.h"

uint8_t recvCache[4];
uint8_t recvPos;

uint16_t recvAddr;

uint8_t viewTimeout = 0;
#define TIMEOUT_MAX	200

uint8_t decode48Code(uint8_t code);
void railcomDecode();

void uartReceive(uint8_t data) {
	uint8_t data2;
	
	
	
	data2 = decode48Code(data);
	
	if ((data2 & 0xF0) == 0xF0) {
		if (recvPos != 0) railcomDecode();
		recvPos = 0;
		return;
	}
	if (recvPos >= 4) return;
	
	recvCache[recvPos] = data2;
	recvPos++;
	
}

uint8_t decode48Code(uint8_t code) {

	if (code == 0b10101100) return (0x00);
	if (code == 0b10101010) return (0x01);
	if (code == 0b10101001) return (0x02);
	if (code == 0b10100101) return (0x03);
	if (code == 0b10100011) return (0x04);
	if (code == 0b10100110) return (0x05);
	if (code == 0b10011100) return (0x06);
	if (code == 0b10011010) return (0x07);
	if (code == 0b10011001) return (0x08);
	if (code == 0b10010101) return (0x09);
	if (code == 0b10010011) return (0x0A);
	if (code == 0b10010110) return (0x0B);
	if (code == 0b10001110) return (0x0C);
	if (code == 0b10001101) return (0x0D);
	if (code == 0b10001011) return (0x0E);
	if (code == 0b10110001) return (0x0F);
	
	if (code == 0b10110010) return (0x10);
	if (code == 0b10110100) return (0x11);
	if (code == 0b10111000) return (0x12);
	if (code == 0b01110100) return (0x13);
	if (code == 0b01110010) return (0x14);
	if (code == 0b01101100) return (0x15);
	if (code == 0b01101010) return (0x16);
	if (code == 0b01101001) return (0x17);
	if (code == 0b01100101) return (0x18);
	if (code == 0b01100011) return (0x19);
	if (code == 0b01100110) return (0x1A);
	if (code == 0b01011100) return (0x1B);
	if (code == 0b01011010) return (0x1C);
	if (code == 0b01011001) return (0x1D);
	if (code == 0b01010101) return (0x1E);
	if (code == 0b01010011) return (0x1F);
	
	if (code == 0b01010110) return (0x20);
	if (code == 0b01001110) return (0x21);
	if (code == 0b01001101) return (0x22);
	if (code == 0b01001011) return (0x23);
	if (code == 0b01000111) return (0x24);
	if (code == 0b01110001) return (0x25);
	if (code == 0b11101000) return (0x26);
	if (code == 0b11100100) return (0x27);
	if (code == 0b11100010) return (0x28);
	if (code == 0b11010001) return (0x29);
	if (code == 0b11001001) return (0x2A);
	if (code == 0b11000101) return (0x2B);
	if (code == 0b11011000) return (0x2C);
	if (code == 0b11010100) return (0x2D);
	if (code == 0b11010010) return (0x2E);
	if (code == 0b11001010) return (0x2F);
	
	if (code == 0b11000110) return (0x30);
	if (code == 0b11001100) return (0x31);
	if (code == 0b01111000) return (0x32);
	if (code == 0b00010111) return (0x33);
	if (code == 0b00011011) return (0x34);
	if (code == 0b00011101) return (0x35);
	if (code == 0b00011110) return (0x36);
	if (code == 0b00101110) return (0x37);
	if (code == 0b00110110) return (0x38);
	if (code == 0b00111010) return (0x39);
	if (code == 0b00100111) return (0x3A);
	if (code == 0b00101011) return (0x3B);
	if (code == 0b00101101) return (0x3C);
	if (code == 0b00110101) return (0x3D);
	if (code == 0b00111001) return (0x3E);
	if (code == 0b00110011) return (0x3F);
	
	if (code == 0x0F) return (0xF1);	// NACK
	if (code == 0xF0) return (0xF0);	// ACK
	if (code == 0xE1) return (0xF2);	// BUSY
	
	return (0xFF);
	
}


void railcomDecode() {
	uint8_t ID;
	uint8_t data;
	
	if (recvPos == 2) {
		ID = (recvCache[0] & 0x3C) >> 2;
		data = ((recvCache[0] & 0x03) << 6) + (recvCache[1] & 0x3F);
		
		if (ID == 0x01) {
			// High (ID1)
			if ((data & 0xC0) == 0x80) {
				// Long Addr
				recvAddr += (uint16_t)(data & 0x3F) << 8;
			}
			// Both Long Addr and Short Addr
			set_led_power_stat(0);
			set_7seg_data(recvAddr);
			viewTimeout = 0;
		} else if (ID == 0x02) {
			// Low (ID2)
			recvAddr = (uint16_t)data;
		}
	}
}

void timeoutCountup(void) {
	if (viewTimeout <= TIMEOUT_MAX) {
		viewTimeout++;
	}
}

uint8_t addrViewTimeout(void) {
	if (viewTimeout > TIMEOUT_MAX) {
		return (1);
	}
	return (0);
}