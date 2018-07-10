#ifndef _KITCO_H_
#define _KITCO_H_

typedef enum {
	A = 1,
	B = 2,
	UP = 4,
	LEFT = 16,
	DOWN = 32,
	RIGHT = 64
} kitco_button;

void kitco_init(void);
unsigned char kitco_button_states();

#endif
