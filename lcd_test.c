#include "lcd.h"
#include "lcd_test.h"
// #include "lcd.h"
// #include "LCD.c"

void test_LCD(void)
{
	LCDClear();
    InitLCD(LS_BLINK|LS_ULINE);
	LCDClear();
	LCDWriteString("LCD Testing");
}


