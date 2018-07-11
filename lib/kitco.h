#ifndef _KITCO_H_
#define _KITCO_H_

typedef enum {
	KITCO_BTN_A,
	KITCO_BTN_B ,
	KITCO_BTN_UP,
	KITCO_BTN_LEFT,
	KITCO_BTN_DOWN,
	KITCO_BTN_RIGHT
} kitco_button;

void kitco_init(void);
unsigned char kitco_button_states();

#endif
