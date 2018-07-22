#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include <kitco.h>
#include <kitco_nokia.h>
#include <kitco_gfx.h>
#include "pidgeot.h"
#include "pikachu.h"

kitco_video_vram vram;
volatile uint16_t offset, toggle=0;

int main(void) {

	kitco_init();
	kitco_lcd_checkerboard();
	kitco_lcd_buffer_create(&vram, 2);
	kitco_lcd_buffer_clean(&vram, 0);
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
			copy_image((toggle ? &pidgeot : &pikachu),
					0,0,
					toggle ? 60 : 84, toggle ? 30 : 48,
					toggle ? 0 : 30, toggle ? 0 : 15,
					&vram);
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
	kitco_lcd_buffer_draw(&vram);
}


ISR (TIMER1_OVF_vect)
{
	if( offset == 0 )
	{
		//PORTC ^= _BV(KITCO_LEDGREEN);
		kitco_lcd_buffer_draw(&vram);
	}
	offset++;
	offset%=50;
}

