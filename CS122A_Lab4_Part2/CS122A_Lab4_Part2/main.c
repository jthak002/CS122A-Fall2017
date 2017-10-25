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
        led_state=L0;
        break;
    }
    //Actions
    switch(led_state){
        case L0:
        PORTD = 0x15;
        break;
        case L1:
        PORTD = 0x00;
        break;
        default:
        PORTD = 0;
        break;
    }
}

void LedSecTask()
{
    LEDS_Init();
    for(;;)
    {
        LEDS_Tick();
        vTaskDelay(1000);
    }
}

void StartSecPulse(unsigned portBASE_TYPE Priority)
{
    xTaskCreate(LedSecTask, (signed portCHAR *)"LedSecTask", configMINIMAL_STACK_SIZE, NULL, Priority, NULL );
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