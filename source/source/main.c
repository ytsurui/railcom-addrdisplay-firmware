#include <avr/io.h>
#include <avr/interrupt.h>

#include "portdef.h"
#include "common.h"
#include "led_controller.h"
//#include "loconet.h"
#include "eeprom_main.h"
#include "uart.h"
#include "railcom_recv.h"

#define NO_RAILCOM_COUNT_MAX 2000

int main(void)
{
	//uint8_t rcv_data;
	
	uint16_t noRailcomCounter = 0;
	uint16_t railcomFlushCount = 0;
	
	DDRB = 0xFF;
	//DDRD = 0xFF & ~CONFIG_JUMPER;
	DDRD = 0xFF;
	
	PORTB = 0x00;
	PORTD = 0x00;
	
	//CONFIG_JUMPER_PORT |= CONFIG_JUMPER;
		
	//read_addr();

	set_led_power_stat(1);
	set_7seg_timer();
	
	init_uart(250000);
	//init_loconet_timer_interrupt();
	
	
	//rcv_data = 0;
	
	while(1)
	{
		/*
		delay_ms(250);
		set_7seg_data(rcv_data);
		rcv_data++;
		*/
		
		if (appTimer()) {
			timeoutCountup();
			
			if (noRailcomCounter < NO_RAILCOM_COUNT_MAX) {
				noRailcomCounter++;
				if (addrViewTimeout()) {
					set_led_power_stat(0);
					railcomFlushCount++;
					if (railcomFlushCount == 256) {
						set_null_msg(0);
					} else if (railcomFlushCount == 512) {
						set_null_msg(1);
					} else if (railcomFlushCount == 768) {
						set_null_msg(2);
					} else if (railcomFlushCount == 1024) {
						set_null_msg(3);
						railcomFlushCount = 0;
					} else if (railcomFlushCount > 1024) {
						railcomFlushCount = 0;
					}
				}
			} else {
				railcomFlushCount++;
				if (railcomFlushCount == 256) {
					set_led_power_stat(0);
					set_null_msg(0);
				} else if (railcomFlushCount == 512) {
					set_led_power_stat(1);
					railcomFlushCount = 0;
				} else if (railcomFlushCount > 512) {
					railcomFlushCount = 0;
				}
			}
		}
		
		if (uart_rcv_size()) {
			uartReceive(uart_getch());
			noRailcomCounter = 0;
			//railcomFlushCount = 0;
		}
		
		if (noRailcomCounter > NO_RAILCOM_COUNT_MAX) {
			
		}
		
		
		/*
		if (~CONFIG_JUMPER_PIN & CONFIG_JUMPER) {
			//PROG_MODE
			set_prog_mode();
			set_led_power_stat(0);
			
			while (~CONFIG_JUMPER_PIN & CONFIG_JUMPER) {
				if (uart_rcv_size()) {
					//loconet_recv(uart_getch());
					uart_getch();
				}
			}
		} else {
			//NORMAL_MODE
			read_addr();
			set_led_power_stat(1);
			
			while (CONFIG_JUMPER_PIN & CONFIG_JUMPER) {
				if (uart_rcv_size()) {
					//loconet_recv((uart_getch()));
				}
			}	
		}
		*/
        
	}
}