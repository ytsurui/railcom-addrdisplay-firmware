#include <avr/io.h>
#include <avr/interrupt.h>

#include "portdef.h"
#include "common.h"
#include "led_controller.h"
//#include "loconet.h"
#include "eeprom_main.h"
#include "uart.h"
#include "railcom_recv.h"

int main(void)
{
	//uint8_t rcv_data;
	
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
			if (addrViewTimeout()) {
				set_led_power_stat(1);
			}
		}
		
		if (uart_rcv_size()) {
			uartReceive(uart_getch());
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