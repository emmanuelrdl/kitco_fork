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

#define NOKIA_WIDTH      84
#define NOKIA_HEIGHT     48
#define BUF_LEN			 NOKIA_WIDTH * NOKIA_HEIGHT / 8

typedef struct {
	volatile unsigned char * data;
	unsigned char depth;
	unsigned char phase;
} kitco_video_vram;

void kitco_lcd_init();
void kitco_lcd_checkerboard();
void kitco_lcd_buffer_create(kitco_video_vram *buf, unsigned char depth);
void kitco_lcd_buffer_destroy(kitco_video_vram *buf);
void kitco_lcd_buffer_clean(kitco_video_vram *buf, unsigned char color);
void kitco_lcd_buffer_draw(kitco_video_vram *buf);
void kitco_lcd_timer1();
void kitco_lcd_timer2();

#ifdef __cplusplus
}
#endif

#endif /* KITCO_NOKIA_H_ */
