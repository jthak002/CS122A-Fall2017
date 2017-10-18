/*
 * Lab_1.c
 *
 * Created: 10-10-2017 12:01:03
 * Author : Jeet
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include "C:\includes\usart.h"


volatile unsigned char TimerFlag = 0;
unsigned long avr_timer_M = 1;
unsigned long avr_timer_cntcurr = 0;

unsigned char valB = 0x00;
void TimerOn(){
    TCCR1B = 0x0B;
    OCR1A = 125;
    TIMSK1 = 0x02;
    TCNT1 = 0;
    avr_timer_cntcurr = avr_timer_M;
    SREG = 0x80;
}

void TimerOff(){
    TCCR1B = 0x00;
}

void TimerISR(){
    TimerFlag = 1;
}

ISR(TIMER1_COMPA_vect){
    avr_timer_cntcurr--;
    if (avr_timer_cntcurr == 0){
        TimerISR();
        avr_timer_cntcurr = avr_timer_M;
        
    }
}

void TimerSet(unsigned long M){
    avr_timer_M = M;
    avr_timer_cntcurr = avr_timer_M;
    
}

enum State_1{Start_1, DATA_TRANSMIT,DATA_RECEIVE} SM_1;
unsigned char supposed_b_val=0;
unsigned char leader=1;
unsigned char port_b_val=0;
void Tick_SM_1(){
    switch(SM_1){
        case Start_1:
            SM_1=DATA_TRANSMIT;
        break;

        case DATA_TRANSMIT:
            if(leader){
                SM_1=DATA_TRANSMIT;
            }
            else{
                SM_1=DATA_RECEIVE;
            }
        break;

        case DATA_RECEIVE:
            if(leader){
                SM_1=DATA_TRANSMIT;
            }
            else{
                SM_1=DATA_RECEIVE;
            }      
        break;

        default:break;
    }
    switch(SM_1){
        case Start_1:
        break;

        case DATA_TRANSMIT:
            if(USART_IsSendReady(1))
            {
                USART_Send(supposed_b_val,1);
            }
            port_b_val=supposed_b_val;
        break;

        case DATA_RECEIVE:
            if(USART_HasReceived(0))
            {
                port_b_val=USART_Receive(0);
                USART_Flush(0);
            }
        break;

        default:break;
    }
}

enum State_2{Start_2, CHANGE_LED}SM_2;
unsigned char counter_val=0x00;
void Tick_SM_2(){
    switch(SM_2){
        case Start_2:
            SM_2=CHANGE_LED;
            break;
        case CHANGE_LED:
            SM_2=CHANGE_LED;
            break;
        default:break;
    }
    switch(SM_2){
        case CHANGE_LED:
            if(counter_val<99)
                counter_val++;
            else{
                if(supposed_b_val==0x00)
                    supposed_b_val=0x01;
                else
                    supposed_b_val=0x00;
                counter_val=0;
            }
            break;
        default:break;
    }
  
}
enum State_3{Start_3,LED_DISPLAY}SM_3;
void Tick_SM_3(){
    switch(SM_3){
        case Start_3:
            SM_3=LED_DISPLAY;
            break;
        case LED_DISPLAY:
            SM_3=LED_DISPLAY;
            break;
        default:break;
    }
    switch(SM_3){
        case LED_DISPLAY:
            PORTB=port_b_val;
            if(leader)
                PORTC=0xFF;
            else
                PORTC=0x00;
            break;
        default:break;
    }
}
unsigned char pin_a_value;
enum State_4{Start_4,CHECK_LEADER_SWITCH}SM_4;
void Tick_SM_4(){
    switch(SM_4){
        case Start_4:
            SM_4=CHECK_LEADER_SWITCH;
            break;
        case CHECK_LEADER_SWITCH:
            SM_4=CHECK_LEADER_SWITCH;
            break;
        default:break;
    }
    switch(SM_4){
        case CHECK_LEADER_SWITCH:
            pin_a_value=PINA;
            if( (pin_a_value&0x01)==0x01)
                leader=0x01;
            else
                leader=0x00;
            break;
        default:break;
    }
}
int main(void)
{
    DDRA=0x00;PORTA=0xFF;
    DDRC=0xFF;PORTC=0x00;
    DDRB = 0xFF;PORTB = 0x00;
    initUSART(0);
    initUSART(1);
    TimerSet(10);
    TimerOn();
    
    SM_1=Start_1;
    SM_2=Start_2;
    SM_3=Start_3;
    SM_4=Start_4;
    while(1)
    {
        Tick_SM_4();
        Tick_SM_1();
        Tick_SM_2();
        Tick_SM_3();
        while(!TimerFlag);
        TimerFlag = 0;
    }
}