#include <stdint.h> 
#include <stdlib.h> 
#include <stdio.h> 
#include <stdbool.h> 
#include <string.h> 
#include <math.h> 
#include <avr/io.h> 
#include <avr/interrupt.h> 
#include <avr/eeprom.h> 
#include <avr/portpins.h> 
#include <avr/pgmspace.h> 
 
//FreeRTOS include files 
#include "FreeRTOS.h" 
#include "task.h" 
#include "croutine.h" 
enum LEDState {INIT,L0,L1} led_state;

void LEDS_Init(){
	led_state = INIT;
}

void LEDS_Tick(){
    	//Transitions
    	switch(led_state){
        	case INIT:
        	led_state = L0;
        	break;
        	case L0:
        	led_state = L1;
        	break;
        	case L1:
        	led_state = L0;
        	break;
        	default:
        	led_state = INIT;
        	break;
    	}
	//Actions
	switch(led_state){
		case INIT:
		PORTD = 0;
		break;
		case L0:
		PORTD = PORTD|0x01;
		break;
		case L1:
		PORTD = PORTD&0xFE;
		break;
		default:break;
	}
}

void LedSecTask()
{
	LEDS_Init();
   for(;;) 
   { 	
	LEDS_Tick();
	vTaskDelay(500); 
   } 
}

enum LEDState1 {INIT1,L01,L11} led_state1;

void LEDS_Init1(){
    led_state1 = INIT1;
}

void LEDS_Tick1(){
    //Transitions
    switch(led_state1){
        case INIT1:
        led_state1 = L01;
        break;
        case L01:
        led_state1 = L11;
        break;
        case L11:
        led_state1 = L01;
        break;
        default:
        led_state1 = INIT1;
        break;
    }
    //Actions
    switch(led_state1){
        case INIT1:
        PORTD = 0;
        break;
        case L01:
        PORTD = PORTD|0x04;
        break;
        case L11:
        PORTD = PORTD&0xFB;
        break;
        default:break;
    }
}
void LedSecTask1()
{
    LEDS_Init1();
    for(;;)
    {
        LEDS_Tick1();
        vTaskDelay(1000);
    }
}

enum LEDState2 {INIT2,L02,L12} led_state2;

void LEDS_Init2(){
    led_state2 = INIT2;
}

void LEDS_Tick2(){
    //Transitions
    switch(led_state2){
        case INIT2:
        led_state2 = L02;
        break;
        case L02:
        led_state2 = L12;
        break;
        case L12:
        led_state2 = L02;
        break;
        default:
        led_state2 = INIT2;
        break;
    }
    //Actions
    switch(led_state2){
        case INIT2:
        PORTD = 0;
        break;
        case L02:
        PORTD = PORTD|0x10;
        break;
        case L12:
        PORTD = PORTD&0xEF;
        break;
        default:break;
    }
}
void LedSecTask2()
{
    LEDS_Init2();
    for(;;)
    {
        LEDS_Tick2();
        vTaskDelay(1500);
    }
}

void StartSecPulse(unsigned portBASE_TYPE Priority)
{
	xTaskCreate(LedSecTask, (signed portCHAR *)"LedSecTask", configMINIMAL_STACK_SIZE, NULL, Priority, NULL );
    xTaskCreate(LedSecTask1, (signed portCHAR *)"LedSecTask1", configMINIMAL_STACK_SIZE, NULL, Priority, NULL );
    xTaskCreate(LedSecTask2, (signed portCHAR *)"LedSecTask2", configMINIMAL_STACK_SIZE, NULL, Priority, NULL );
}	
 
int main(void) 
{ 
   DDRA = 0x00; PORTA=0xFF;
   DDRD = 0xFF;
   //Start Tasks  
   StartSecPulse(1);
    //RunSchedular 
   vTaskStartScheduler(); 
 
   return 0; 
}