#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include <kitco.h>
#include <kitco_nokia.h>
#include <kitco_gfx.h>
#include "pidgeot.h"
#include "pikachu.h"

kitco_video_buffer vidbuf;
volatile uint16_t offset, toggle=0;

int main(void) {

	kitco_init();
	kitco_lcd_checkerboard();
	kitco_lcd_buffer_create(&vidbuf, 1);
	kitco_lcd_buffer_clean(&vidbuf, 0);
	kitco_lcd_timer2();

	while(1)
	{
		if (kitco_button_states() & _BV(KITCO_BTN_B))
			PORTC |= _BV(PC2);
		else
			PORTC &= ~(1 << PC2);

		if (kitco_button_states() & _BV(KITCO_BTN_A))
			PORTC |= _BV(PC3);
		else
			PORTC &= ~(1 << PC3);

		_delay_ms(10);

		if( offset == 0 )
		{
			//(toggle ? &pidgeot : &pikachu)
			copy_image(&pikachu,
					0,0,
					84,48,
					0,0,
					vidbuf);
			toggle = (toggle+1)%2;
		}
		offset++;
		offset%=500;
	}
	return 0;
}


ISR(TIMER2_COMPA_vect)
{
	//PORTC ^= _BV(PC2);
	kitco_lcd_buffer_draw(&vidbuf);
}


ISR (TIMER1_OVF_vect)
{
	if( offset == 0 )
	{
		//PORTC ^= _BV(KITCO_LEDGREEN);
		kitco_lcd_buffer_draw(&vidbuf);
	}
	offset++;
	offset%=50;
}

