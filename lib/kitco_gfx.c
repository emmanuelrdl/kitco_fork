#include "kitco_gfx.h"

#include <avr/pgmspace.h>

void copy_image(const kitco_image_P *img,
		unsigned int to_x,
		unsigned int to_y,
		kitco_video_buffer buf)
{
	// http://teslabs.com/openplayer/docs/docs/prognotes/Progmem%20Tutorial.pdf
	// https://www.microchip.com/webdoc/AVRLibcReferenceManual/pgmspace_1pgmspace_strings.html

	if (to_x >= NOKIA_WIDTH || to_y >= NOKIA_HEIGHT) return;

	unsigned char depth = buf.depth;
	unsigned char width = pgm_read_byte_near(&(img->width));
	unsigned char height = pgm_read_byte_near(&(img->height));

	unsigned char width_copy = width+to_x > NOKIA_WIDTH ? NOKIA_WIDTH-to_x : width;
	unsigned char height_copy = height+to_y > NOKIA_HEIGHT ? NOKIA_HEIGHT-to_y : height;

	unsigned char *data = (unsigned char *)pgm_read_word_near(&(img->data));

	unsigned int index,yoffset;
    for(int x=0; x<width_copy; x++)
    {
    	for(int y=0; y<height_copy; y+=8)
    	{
    		index = (to_x + x + ((y+to_y)/8)*84)*depth;
    		// msb
    		buf.buffer[index] = (3-pgm_read_byte_near(data + x+(y*width))) >> 1 |
    				((3-pgm_read_byte_near(data + x+((y+1)*width))) & 2) |
    				((3-pgm_read_byte_near(data + x+((y+2)*width))) & 2) << 1 |
    				((3-pgm_read_byte_near(data + x+((y+3)*width))) & 2) << 2 |
    				((3-pgm_read_byte_near(data + x+((y+4)*width))) & 2) << 3 |
    				((3-pgm_read_byte_near(data + x+((y+5)*width))) & 2) << 4 |
    				((3-pgm_read_byte_near(data + x+((y+6)*width))) & 2) << 5 |
    				((3-pgm_read_byte_near(data + x+((y+7)*width))) & 2) << 6;

    		/*
    		// msb
    		buf.buffer[index] = (3-pgm_read_byte_near(data + x+(y*width))) >> 1 |
    				((3-pgm_read_byte_near(data + x+((y+1)*width))) & 2) |
    				((3-pgm_read_byte_near(data + x+((y+2)*width))) & 2) << 1 |
    				((3-pgm_read_byte_near(data + x+((y+3)*width))) & 2) << 2 |
    				((3-pgm_read_byte_near(data + x+((y+4)*width))) & 2) << 3 |
    				((3-pgm_read_byte_near(data + x+((y+5)*width))) & 2) << 4 |
    				((3-pgm_read_byte_near(data + x+((y+6)*width))) & 2) << 5 |
    				((3-pgm_read_byte_near(data + x+((y+7)*width))) & 2) << 6;

    		if (depth == 2)
    		{
				// lsb
				buf.buffer[index+1] = ((3-pgm_read_byte_near(data + x+(y*width))) & 1) |
						((3-pgm_read_byte_near(data + x+((y+1)*width))) & 1) << 1 |
						((3-pgm_read_byte_near(data + x+((y+2)*width))) & 1) << 2 |
						((3-pgm_read_byte_near(data + x+((y+3)*width))) & 1) << 3 |
						((3-pgm_read_byte_near(data + x+((y+4)*width))) & 1) << 4 |
						((3-pgm_read_byte_near(data + x+((y+5)*width))) & 1) << 5 |
						((3-pgm_read_byte_near(data + x+((y+6)*width))) & 1) << 6 |
						((3-pgm_read_byte_near(data + x+((y+7)*width))) & 1) << 7;
    		}
    		*/
    	}
    }
}
