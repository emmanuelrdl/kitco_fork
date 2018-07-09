#ifndef _KITCO_H_
#define _KITCO_H_

#include "kitco_nokia.h"

#define KITCO_LEDGREEN PC3
#define KITCO_LEDRED PC2

void kitco_init(void)
{
	DDRC |= _BV(KITCO_LEDGREEN);
	DDRC |= _BV(KITCO_LEDRED);
	kitco_lcd_init();
}

#endif
