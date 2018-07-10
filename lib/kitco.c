/*
 * kitco.c
 *
 *  Created on: 8 juil. 2018
 *      Author: chris
 */
#include <avr/io.h>
#include "kitco_nokia.h"

#define KITCO_LEDGREEN PC3
#define KITCO_LEDRED PC2

#define KITCO_BTN_A PD2
#define KITCO_BTN_B PD3
#define KITCO_BTN_UP PD4
#define KITCO_BTN_LEFT PD6
#define KITCO_BTN_DOWN PD7
#define KITCO_BTN_RIGHT PB0

#define KITCO_BUZZER PC0

void kitco_init(void)
{
	DDRC |= _BV(KITCO_LEDGREEN);
	DDRC |= _BV(KITCO_LEDRED);

	// pull ups
	PORTD |= _BV(KITCO_BTN_A);
	PORTD |= _BV(KITCO_BTN_B);
	PORTD |= _BV(KITCO_BTN_UP);
	PORTD |= _BV(KITCO_BTN_LEFT);
	PORTD |= _BV(KITCO_BTN_DOWN);
	PORTB |= _BV(KITCO_BTN_RIGHT);

	kitco_lcd_init();
}

unsigned char kitco_button_states()
{
	return (PIND >> 2) | ((PINB & _BV(PB0)) << 6);
}
