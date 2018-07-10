/*
 * kitco_nokia.h
 *
 *  Created on: 7 juil. 2018
 *      Author: chris
 */

#ifndef KITCO_NOKIA_H_
#define KITCO_NOKIA_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	volatile unsigned char * buffer;
	volatile unsigned char depth;
	volatile unsigned char phase;
} kitco_video_buffer;

void kitco_lcd_init();
void kitco_lcd_checkerboard();
void kitco_lcd_buffer_create(kitco_video_buffer *buf, unsigned char depth);
void kitco_lcd_buffer_destroy(kitco_video_buffer *buf);
void kitco_lcd_buffer_clean(kitco_video_buffer *buf, unsigned char color);
void kitco_lcd_buffer_draw(kitco_video_buffer *buf);
void kitco_lcd_timer1();
void kitco_lcd_timer2();

#ifdef __cplusplus
}
#endif

#endif /* KITCO_NOKIA_H_ */
