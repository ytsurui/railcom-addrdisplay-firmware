#include <avr/io.h>
#include <util/delay.h>

void delay_ms(uint8_t ms)
{
	while (ms--) {
		_delay_ms(1);
	}
}