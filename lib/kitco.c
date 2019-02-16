/*
 * kitco.c
 *
 *  Created on: 8 juil. 2018
 *      Author: chris
 */
#include <avr/io.h>
#include "kitco.h"
#include "kitco_nokia.h"

#define KITCO_LEDGREEN PC3
#define KITCO_LEDRED PC2

#define KITCO_PIN_A PD2
#define KITCO_PIN_B PD3
#define KITCO_PIN_UP PD4
#define KITCO_PIN_LEFT PD6
#define KITCO_PIN_DOWN PD7
#define KITCO_PIN_RIGHT PB0

#define KITCO_BUZZER PC0

void kitco_init(void)
{
	DDRC |= _BV(KITCO_LEDGREEN);
	DDRC |= _BV(KITCO_LEDRED);

	// pull ups
	PORTD |= _BV(KITCO_PIN_A);
	PORTD |= _BV(KITCO_PIN_B);
	PORTD |= _BV(KITCO_PIN_UP);
	PORTD |= _BV(KITCO_PIN_LEFT);
	PORTD |= _BV(KITCO_PIN_DOWN);
	PORTB |= _BV(KITCO_PIN_RIGHT);

	kitco_lcd_init();
}

unsigned char kitco_button_states()
{
	uint8_t pind = PIND;
	return ~(((pind >> KITCO_PIN_A) & 1) << KITCO_BTN_A |
			((pind >> KITCO_PIN_B) & 1) << KITCO_BTN_B |
			((pind >> KITCO_PIN_UP) & 1) << KITCO_BTN_UP |
			((pind >> KITCO_PIN_LEFT) & 1) << KITCO_BTN_LEFT |
			((pind >> KITCO_PIN_DOWN) & 1) << KITCO_BTN_DOWN |
			((PINB >> KITCO_PIN_RIGHT) & 1) << KITCO_BTN_RIGHT);
}
