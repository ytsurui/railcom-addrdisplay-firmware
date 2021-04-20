/*
 * railcom_recv.c
 *
 * Created: 2020/12/29 0:27:55
 *  Author: ytsurui
 */ 

#include <avr/io.h>
#include <avr/pgmspace.h>
#include "led_controller.h"

uint8_t recvCache[4];
uint8_t recvPos;

#define RECV_ADDR_CACHE_MAX 2
uint16_t recvAddrCache[RECV_ADDR_CACHE_MAX];
uint8_t recvAddrPos;

uint16_t recvAddr;

uint16_t viewTimeout = 0;
#define TIMEOUT_MAX	1000

const PROGMEM uint8_t encodeDataTable[64] = {	0xAC, 0xAA, 0xA9, 0xA5, 0xA3, 0xA6, 0x9C, 0x9A,
												0x99, 0x95, 0x93, 0x96, 0x8E, 0x8D, 0x8B, 0xB1,
												0xB2, 0xB4, 0xB8, 0x74, 0x72, 0x6C, 0x6A, 0x69,
												0x65, 0x63, 0x66, 0x5C, 0x5A, 0x59, 0x55, 0x53,
												0x56, 0x4E, 0x4D, 0x4B, 0x47, 0x71, 0xE8, 0xE4,
												0xE2, 0xD1, 0xC9, 0xC5, 0xD8, 0xD4, 0xD2, 0xC9,
												0xC6, 0xCC, 0x78, 0x17, 0x1B, 0x1D, 0x1E, 0x2E,
												0x36, 0x3A, 0x27, 0x2B, 0x2D, 0x35, 0x39, 0x33};

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
	uint8_t i;
	
	if (code == 0x0F) return (0xF1);	// NACK
	if (code == 0xF0) return (0xF0);	// ACK
	if (code == 0xE1) return (0xF2);	// BUSY
	
	for (i = 0; i < 64; i++) {
		if (code == pgm_read_byte(&encodeDataTable[i])) {
			return (i);
		}
	}
	
	return (0xFF);
	
}

uint8_t checkRecvAddrCache(uint16_t recvAddr) {
	uint8_t i;
	for (i = 0; i < RECV_ADDR_CACHE_MAX; i++) {
		if (recvAddrCache[i] != recvAddr) {
			return (0);
		}
	}
	return (1);
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
			if (checkRecvAddrCache(recvAddr)) {
				set_led_power_stat(0);
				set_7seg_data(recvAddr);
				viewTimeout = 0;
			}
			
			recvAddrCache[recvAddrPos] = recvAddr;
			recvAddrPos++;
			if (recvAddrPos >= RECV_ADDR_CACHE_MAX) recvAddrPos = 0;
			
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