#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "protocol/serial.h"
#include "../config.h"

void serial_init() {
	SERIAL_SOFT_TX_PORT |= (1 << SERIAL_SOFT_TX_BIT);
	SERIAL_SOFT_TX_DDR |= (1 << SERIAL_SOFT_TX_BIT);
}

void serial_send(uint8_t c) {
	uint8_t bit_mask;
	// start bit
	SERIAL_SOFT_TX_PORT &= ~(1 << SERIAL_SOFT_TX_BIT);
	_delay_us(SERIAL_SOFT_MICROSECONDS_PER_BIT);
	// data bits
	for (bit_mask = 0x01; bit_mask; bit_mask <<= 1) {
		if (c & bit_mask) {
			SERIAL_SOFT_TX_PORT |= (1 << SERIAL_SOFT_TX_BIT);
		} else {
			SERIAL_SOFT_TX_PORT &= ~(1 << SERIAL_SOFT_TX_BIT);
		}
		_delay_us(SERIAL_SOFT_MICROSECONDS_PER_BIT);
	}
	// stop bit(s)
	SERIAL_SOFT_TX_PORT |= (1 << SERIAL_SOFT_TX_BIT);
	_delay_us(SERIAL_SOFT_MICROSECONDS_PER_BIT * SERIAL_SOFT_STOP_BITS);
}

