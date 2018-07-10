/*
 * kitco_nokia.c
 *
 *  Created on: 7 juil. 2018
 *      Author: chris
 */
#define LCD_PWM_V2

#include "kitco_nokia.h"
#include "kitco_nokia_chars.h"
#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <string.h>
#include <avr/interrupt.h>

#define NOKIACS PB1
#define NOKIADC PB2
#define NOKIASD PB3
#define NOKIACL PB5
#define NOKIARST PB7

#define NOKIALIGHT PD5

#define NOKIA_WIDTH      84
#define NOKIA_HEIGHT     48

#define kitco_lcd_enable() PORTB &= ~_BV(NOKIACS)
#define kitco_lcd_disable() PORTB |= _BV(NOKIACS)
//#define kitco_lcd_enable()
//#define kitco_lcd_disable()
#define kitco_lcd_setdata() PORTB |= _BV(NOKIADC)
#define kitco_lcd_setcommand() PORTB &= ~_BV(NOKIADC)
#define kitco_lcd_raw(b) SPDR = b; while(!(SPSR & (1<<SPIF) ));

// -----------------------------

const unsigned int BUF_LEN =  NOKIA_WIDTH * NOKIA_HEIGHT / 8;
const unsigned int BUF_LEN2 =  BUF_LEN * 2;

void kitco_lcd_timer2()
{
	OCR2A = 90;
	TCCR2A |= (1 << WGM21); // Set to CTC Mode
	TIMSK2 |= (1 << OCIE2A); //Set interrupt on compare match
	TCCR2B |= (1 << CS21) | (1 << CS20) | (1 << CS22); // set prescaler to 1024 and starts PWM
	// freq = 8MHz/(1024*32) = 244Hz (4ms)
	sei();
}

void kitco_lcd_timer1()
{
	ICR1 = 12195;
	TCCR1A = 0;
	TCCR1B = _BV(WGM13) | _BV(CS11); // PWM, Phase and Frequency Correct + fosc/8

	//TCCR1A |= _BV(COM1A1);
	//OCR1A = 6097;

	TCCR1B = _BV(WGM13) | _BV(CS11);
	TIMSK1 = _BV(TOIE1);
	sei();
}

void kitco_lcd_buffer_create(kitco_video_buffer *buf, unsigned char depth)
{
	if (buf->depth == 0) // initialized ?
	{
		buf->depth = depth;
		buf->phase = 0;
		buf->buffer = malloc(depth * BUF_LEN * sizeof(unsigned char));
	}
}

void kitco_lcd_buffer_destroy(kitco_video_buffer *buf)
{
	free((unsigned char *)buf->buffer);
	buf->depth=0;
}

void kitco_lcd_buffer_clean(kitco_video_buffer *buf, unsigned char color)
{
	memset((unsigned char *)buf->buffer, color, BUF_LEN*buf->depth);
}

void kitco_lcd_send_data(unsigned char data)
{
	kitco_lcd_enable();
	kitco_lcd_setdata();
	_delay_us(10);
	kitco_lcd_raw(data);
	kitco_lcd_disable();
}

void kitco_lcd_send_command(unsigned char data)
{
	kitco_lcd_enable();
	kitco_lcd_setcommand();
	_delay_us(10);
	kitco_lcd_raw(data);
	kitco_lcd_disable();
}

void kitco_lcd_buffer_draw(kitco_video_buffer *buf)
{
	kitco_lcd_enable();
	kitco_lcd_setcommand();
	_delay_us(10);
	kitco_lcd_raw(0x40);
	kitco_lcd_raw(0x80);
	kitco_lcd_setdata();
	_delay_us(10);

	unsigned char phase = buf->phase;
	unsigned char *buffer = (unsigned char *)buf->buffer;
	if(buf->depth == 2)
	{
		if (phase == 0)
			for (int i=0; i < BUF_LEN2; i+=2) {
				kitco_lcd_raw(buffer[i]|buffer[i+1]);	// | ---|
			}
#ifndef LCD_PWM_V2
		else if (phase == 1)
#else
		else if (phase == 2)
#endif
			for(int i=0; i<BUF_LEN2; i+=2) {
				kitco_lcd_raw(buffer[i]);				// |  --|
			}
		else
			for (int i=0; i < BUF_LEN2; i+=2) {
				kitco_lcd_raw(buffer[i]&buffer[i+1]);	// |   -|
			}
	}
	else
		for (int i=0; i < BUF_LEN; i++) {
			kitco_lcd_raw(buffer[i]);
		}

#ifndef LCD_PWM_V2
	buf->phase = (phase + 1)%3;
#else
	buf->phase = (phase + 1)%4;
#endif
	kitco_lcd_disable();
}

void kitco_lcd_checkerboard()
{
	kitco_lcd_enable();
	kitco_lcd_setcommand();
	_delay_us(10);
	kitco_lcd_raw(0x40);
	kitco_lcd_raw(0x80);
	kitco_lcd_setdata();
	_delay_us(10);
	for (int i=0; i < BUF_LEN; i++) {
		kitco_lcd_raw( ((i%NOKIA_WIDTH)/9)%2 == (i/NOKIA_WIDTH)%2 ? 0xff : 0);
	}
	kitco_lcd_disable();
}

void kitco_lcd_init()
{
	DDRB |= _BV(NOKIACS);
	DDRB |= _BV(NOKIADC);
	DDRB |= _BV(NOKIASD);
	DDRB |= _BV(NOKIACL);
	DDRB |= _BV(NOKIARST);

	DDRD |= _BV(NOKIALIGHT);
	PORTD &= ~(1 << NOKIALIGHT); // backlight on

	// spi init + master + MSB + "SPI_MODE0" + 8MHz/2
	SPCR = _BV(SPE) | _BV(MSTR);
	SPSR = _BV(SPI2X);

	// reset lcd
	PORTB |= (1 << NOKIARST);
	PORTB |= (1 << NOKIACS);
	_delay_ms(10);
	PORTB &= ~(1 << NOKIARST); // actual reset
	_delay_ms(100);
	PORTB |= (1 << NOKIARST);
	PORTB &= ~(1 << NOKIACS);

	// temp Vop bias
	// 0x6 0xb7 0x14
	// 0x4 0xa3 0x14
	// 0x4 0x98 0x15
	// 0x4 0x88 0x16
	// 0x4 0x78 0x17

	kitco_lcd_send_command(0x21); //Tell LCD extended commands follow
	kitco_lcd_send_command(0x14); //LCD bias mode 1:48 (try 0x13)
	kitco_lcd_send_command(0x04); //Set Temp coefficent
	kitco_lcd_send_command(0xa5); //Set LCD Vop (Contrast)
	kitco_lcd_send_command(0x20);
    kitco_lcd_send_command(0x09); // all display segments on

	kitco_lcd_send_command(0x80); // x=0
	kitco_lcd_send_command(0x40); // y=0
	for (int i = 0; i < BUF_LEN; i++)
		kitco_lcd_send_data(0x00);

	kitco_lcd_send_command(0x08); // display blank
	kitco_lcd_send_command(0x0C); // normal mode
}
