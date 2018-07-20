#include "kitco_gfx.h"

#include <avr/pgmspace.h>

#define MAX(a,b) ((a) > (b) ? a : b)
#define MIN(a,b) ((a) < (b) ? a : b)

int copy_image(const kitco_image_P *img,
		unsigned char from_x,
		unsigned char from_y,
		unsigned char copy_width,
		unsigned char copy_height,
		unsigned char to_x,
		unsigned char to_y,
		kitco_video_buffer buf)
{
	// http://teslabs.com/openplayer/docs/docs/prognotes/Progmem%20Tutorial.pdf
	// https://www.microchip.com/webdoc/AVRLibcReferenceManual/pgmspace_1pgmspace_strings.html

	if (to_x >= NOKIA_WIDTH || to_y >= NOKIA_HEIGHT) return -1;

	unsigned char depth = buf.depth;
	unsigned char img_w = pgm_read_byte_near(&(img->width));
	unsigned char img_h = pgm_read_byte_near(&(img->height));

	copy_width = copy_width+to_x > NOKIA_WIDTH ? NOKIA_WIDTH-to_x : copy_width;
	copy_height = copy_height+to_y > NOKIA_HEIGHT ? NOKIA_HEIGHT-to_y : copy_height;

	unsigned char *data = (unsigned char *)pgm_read_word_near(&(img->data));

	unsigned int index;
	unsigned char y=from_y;
	unsigned char y_inc;
	unsigned char y2,bits=0;
    for(int x=from_x; x<copy_width+from_x; x++) // parse image
    {
    	y_inc = 8-(to_y%8); // adjust y increment for the first pixels
    	while (y < copy_height+from_y)
    	{
    		index = to_x + (x-from_x) + (((y-from_y)+to_y)/8)*84; // targeted byte in the vram
    		y2 = MIN(y+y_inc, copy_height+from_y)-1; // crop if we reach bottom of the image

    		if (y2-y != 7) { // partial byte fill -> preload with existing vram bits
    			bits = buf.buffer[index];
    			bits &= ~( (255 >> (7-(y2-y))) << ((y+to_y)%8) );
    		}

    		while (y2 >= y) {
    			bits |= ((pgm_read_byte_near(data + (y2*img_w + x)/8) >> (y2*img_w + x)%8) & 1) << ((y2-from_y)+to_y)%8;
    			y2--;
    		}
    		buf.buffer[index] = bits;

    		y += y_inc;
    		y_inc = 8;
    	}
    }
    return 0;
}
