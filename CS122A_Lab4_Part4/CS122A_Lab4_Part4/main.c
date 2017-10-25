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

unsigned char reverse_flag=0x00;
unsigned char button_release=0x01;
enum ButtonState{INIT1,Normal_State,Check_Press}button_state;
void BUTTONS_Init(){
    button_state=INIT1;
}
void BUTTONS_Tick(){
    //Transition
    switch(button_state)
    {
        case INIT1:
            button_state=Normal_State;
            break;
        case Normal_State:
            if((~PINA&0x01)==0x01)
                button_state=Check_Press;
            else
                button_state=Normal_State;
            break;
        case Check_Press:
            if((~PINA&0x01)==0x01)
            button_state=Check_Press;
            else
            button_state=Normal_State;
            break;
        default:button_state=Check_Press;break;
    }
    //Actions
    switch(button_state)
    {
        case Normal_State:
            PORTB=0x00;
            button_release=0x00;
            break;
        case Check_Press:
            PORTB=0xFF;
            if(!button_release)
            {
                button_release=0x01;
                if(reverse_flag)
                    reverse_flag=0x00;
                else
                    reverse_flag=0x01;
            }                
            break;            
        default:break;
    }
}

void ButtonSecTask()
{
        BUTTONS_Init();
        for(;;)
        {
            BUTTONS_Tick();
            vTaskDelay(100);
        }
}

enum LEDState {INIT,L0} led_state;
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
        if(PORTD!=0x00)
        {
            if(!reverse_flag)
                PORTD=PORTD<<1;
            else
                PORTD=PORTD>>1;
        }
        else{
            if(!reverse_flag)
            {
                PORTD=0x80;
                reverse_flag=0x01;
            }
            else{
                PORTD=0x01;
                reverse_flag=0x00;
            }
        }
        break;
        default:break;
    }
}

void LedSecTask()
{
    PORTD=0x01;
    LEDS_Init();
    for(;;)
    {
        LEDS_Tick();
        vTaskDelay(125);
    }
}

void StartSecPulse(unsigned portBASE_TYPE Priority)
{
    xTaskCreate(LedSecTask, (signed portCHAR *)"LedSecTask", configMINIMAL_STACK_SIZE, NULL, Priority, NULL );
    xTaskCreate(ButtonSecTask, (signed portCHAR *)"ButtonSecTask", configMINIMAL_STACK_SIZE, NULL, Priority, NULL );
}

int main(void)
{
    DDRA = 0x00; PORTA=0xFF;
    DDRB=0xFF; PORTB=0x00;
    DDRD = 0xFF;
    //Start Tasks
    StartSecPulse(1);
    //RunSchedular
    vTaskStartScheduler();
    
    return 0;
}