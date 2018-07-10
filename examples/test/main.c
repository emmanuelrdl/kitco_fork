#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include <kitco.h>
#include <kitco_nokia.h>
#include "pidgeot.h"
#include "pikachu.h"

kitco_video_buffer vidbuf;
volatile uint16_t offset, toggle=0;

void copy_image(const unsigned char *img, unsigned int width, unsigned int height)
{
	unsigned int index=0;

    for(int x=0; x<width; x++)
    {
    	for(int y=0; y<height; y+=8)
    	{
    		index = (x + (y/8)*84)*2;
    		// msb
    		vidbuf.buffer[index] = (3-pgm_read_byte_near(img + x+(y*width))) >> 1 |
    				((3-pgm_read_byte_near(img + x+((y+1)*width))) & 2) |
    				((3-pgm_read_byte_near(img + x+((y+2)*width))) & 2) << 1 |
    				((3-pgm_read_byte_near(img + x+((y+3)*width))) & 2) << 2 |
    				((3-pgm_read_byte_near(img + x+((y+4)*width))) & 2) << 3 |
    				((3-pgm_read_byte_near(img + x+((y+5)*width))) & 2) << 4 |
    				((3-pgm_read_byte_near(img + x+((y+6)*width))) & 2) << 5 |
    				((3-pgm_read_byte_near(img + x+((y+7)*width))) & 2) << 6;
    		// lsb
    		vidbuf.buffer[index+1] = ((3-pgm_read_byte_near(img + x+(y*width))) & 1) |
    				((3-pgm_read_byte_near(img + x+((y+1)*width))) & 1) << 1 |
    				((3-pgm_read_byte_near(img + x+((y+2)*width))) & 1) << 2 |
    				((3-pgm_read_byte_near(img + x+((y+3)*width))) & 1) << 3 |
    				((3-pgm_read_byte_near(img + x+((y+4)*width))) & 1) << 4 |
    				((3-pgm_read_byte_near(img + x+((y+5)*width))) & 1) << 5 |
    				((3-pgm_read_byte_near(img + x+((y+6)*width))) & 1) << 6 |
    				((3-pgm_read_byte_near(img + x+((y+7)*width))) & 1) << 7;
    	}
    }
}

int main(void) {

	kitco_init();
	kitco_lcd_checkerboard();
	kitco_lcd_buffer_create(&vidbuf, 2);
	kitco_lcd_buffer_clean(&vidbuf, 0);
	kitco_lcd_timer2();

	while(1)
	{
		if (kitco_button_states() & B)
			PORTC &= ~(1 << PC2);
		else
			PORTC |= _BV(PC2);

		if (kitco_button_states() & A)
			PORTC &= ~(1 << PC3);
		else
			PORTC |= _BV(PC3);

		_delay_ms(10);

		if( offset == 0 )
		{
			copy_image(
					toggle ? image : pikachu,
					toggle ? image_width : pikachu_width,
					toggle ? image_height : pikachu_height);
			toggle = (toggle+1)%2;
		}
		offset++;
		offset%=100;
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

