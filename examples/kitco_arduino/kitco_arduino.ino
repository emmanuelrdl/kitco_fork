#include <kitco.h>

void setup() {
  // put your setup code here, to run once:
  kitco_lcd_init();
}

void loop() {
  // put your main code here, to run repeatedly:
  kitco_lcd_checkerboard();
}
