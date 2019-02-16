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
		kitco_video_vram *vram)
{
	// http://teslabs.com/openplayer/docs/docs/prognotes/Progmem%20Tutorial.pdf
	// https://www.microchip.com/webdoc/AVRLibcReferenceManual/pgmspace_1pgmspace_strings.html

	if (to_x >= NOKIA_WIDTH || to_y >= NOKIA_HEIGHT) return -1;

	unsigned char *data = (unsigned char *)pgm_read_word_near(&(img->data));
	//unsigned char img_w = pgm_read_byte_near(&(img->width));
	unsigned char img_h = pgm_read_byte_near(&(img->height));
	unsigned char img_d = pgm_read_byte_near(&(img->depth));
	unsigned int img_data1_len = 504;

	copy_width = copy_width+to_x > NOKIA_WIDTH ? NOKIA_WIDTH-to_x : copy_width;
	copy_height = copy_height+to_y > NOKIA_HEIGHT ? NOKIA_HEIGHT-to_y : copy_height;


	unsigned int index;
	unsigned char y;
	unsigned char y_inc;
	unsigned char y2,bits;
    for(unsigned char x=from_x; x<copy_width+from_x; x++) // parse image
    {
    	y = from_y;
    	y_inc = 8-(to_y%8); // adjust y increment for the first pixels
    	while (y < copy_height+from_y)
    	{
    		y2 = MIN(y+y_inc, copy_height+from_y)-1; // crop if we reach bottom of the copy
    		char d = (vram->depth == img_d) ? img_d : 1;

    		while (d-- != 0)
    		{
				index = (d*BUF_LEN) + to_x + (x-from_x) + (((y-from_y)+to_y)/8)*84; // targeted byte in the vram

				unsigned char vram_boffset = ((y-from_y)+to_y)%8;

				if (y2-y != 7) { // partial byte fill -> preload with existing vram bits
					bits = vram->data[index];
					bits &= ~( (255 >> (7-(y2-y))) << vram_boffset );
				}
				else
					bits = 0;

				unsigned int img_i = y + (x*img_h);			// pix index for coord x and y
				unsigned int img_B = img_i/8;				// Byte offset
				unsigned char img_b = img_i%8;				// bit offset
				unsigned char img_bmask = 255 >> (7-(y2-y));// bit mask

				bits |= ((((img_b + y2-y) > 7 ? 			// copied bits overlap two consecutive bytes ?
						pgm_read_word_near(data + img_B + (d*img_data1_len)) : 	// get 2 consecutive bytes (a word)
						pgm_read_byte_near(data + img_B + (d*img_data1_len))	// get 1 byte
				) >> img_b) 								// align with lsb
						& img_bmask) 						// keep bits needed only (start/end of a column)
								<< vram_boffset;	// shift to the right bit position in vram (start of a column)

				vram->data[index] = bits;
    		}

    		y += y_inc;
    		y_inc = 8;
    	}
    }
    return 0;
}
