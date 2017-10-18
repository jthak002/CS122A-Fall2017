/*
 * CS122A_Lab3_LCDCheck.c
 *
 * Created: 17-10-2017 12:06:16
 * Author : Jeet
 */ 

#include <avr/io.h>
#include "C:\includes\io.h"
#include "C:\includes\io.c"


int main(void)
{
    /* Replace with your application code */
    DDRA=0xFF;
    DDRD=0xFF;
    PORTA=0x00;
    PORTD=0x00;
    LCD_init();
    LCD_DisplayString(1,"Hello World!");
    while (1) 
    {
        continue;
    }
}

