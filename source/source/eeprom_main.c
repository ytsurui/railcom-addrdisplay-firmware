#include <avr/eeprom.h>

EEMEM uint16_t section_addr = 1;
uint16_t section_addr_cache;

void read_addr (void)
{
	eeprom_busy_wait();
	section_addr_cache = eeprom_read_word(&section_addr);
}

uint8_t write_addr (uint16_t addr)
{
	if (section_addr_cache == addr) {
		eeprom_busy_wait();
		eeprom_write_word(&section_addr, addr);
		return (1);
	} else {
		section_addr_cache = addr;
		return (0);
	}
}

uint8_t check_addr (uint16_t addr)
{
	if (addr == section_addr_cache) {
		return (1);
	} else {
		return (0);
	}
}