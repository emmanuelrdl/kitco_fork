#ifndef KITCO_GRAPHIC_H_
#define KITCO_GRAPHIC_H_

#include "kitco_nokia.h"
#include <avr/pgmspace.h>

typedef struct {
	unsigned char width;
	unsigned char height;
	unsigned char detph;
	const unsigned char *data;
} kitco_image_P;

typedef struct {

} kitco_gfx_crop;

/*
void copy_image(const unsigned char *img,
		unsigned int width,
		unsigned int height,
		kitco_video_buffer buf);
		*/
void copy_image(const kitco_image_P *img,
		unsigned int x,
		unsigned int y,
		kitco_video_buffer buf);

#endif
