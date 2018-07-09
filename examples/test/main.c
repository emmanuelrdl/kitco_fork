#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include <kitco.h>
#include "pidgeot.h"

kitco_video_buffer vidbuf;

int main(void) {

	kitco_init();
	//kitco_lcd_checkerboard();
	kitco_lcd_buffer_create(&vidbuf, 2);
	kitco_lcd_buffer_clean(&vidbuf, 0);

    for(int x=0; x<image_width; x++)
    {
    	for(int y=0; y<image_height; y+=4)
    	{
    		vidbuf.buffer[(x*2)+((y/4)%2)+((y/8)*84*2)] =
    				(3-pgm_read_byte_near(image + x+(y*image_width))) |
					(3-pgm_read_byte_near(image + x+(y+1)*image_width)) << 2 |
					(3-pgm_read_byte_near(image + x+(y+2)*image_width)) << 4 |
					(3-pgm_read_byte_near(image + x+(y+3)*image_width)) << 6;
    	}
    }

	//kitco_lcd_timer2();

    int ms=10;
    while(1) {
    	kitco_lcd_buffer_draw(&vidbuf);
    	_delay_ms(ms);
    }

}

int offset = 0;
ISR (TIMER2_COMPA_vect)
{
	if( offset == 0 )
	{
		PORTC ^= _BV(KITCO_LEDRED);
		kitco_lcd_buffer_draw(&vidbuf);
	}
	offset++;
	offset%=50;
}
/*
ISR (TIMER1_OVF_vect)
{
	if( offset == 0 )
	{
		PORTC ^= _BV(KITCO_LEDGREEN);
		kitco_lcd_buffer_draw(&vidbuf);
	}
	offset++;
	offset%=50;
}
*/
