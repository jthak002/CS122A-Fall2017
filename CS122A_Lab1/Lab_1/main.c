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


enum States{Start, LED_ON_AND_TRANSMIT} state;
unsigned char port_b_value=0x00;
void Tick(){
    switch (state){
        
        case Start:
        state=LED_ON_AND_TRANSMIT;
        break;
        
        case LED_ON:
        state=LED_ON_AND_TRANSMIT;
        if(port_b_value==0x00)
            port_b_value=0x01;
        else
            port_b_value=0x00;
        break;
        
    }
    
    switch (state){
        
        case LED_ON_AND_TRANSMIT:
        PORTB=port_b_value;
        if(USART_IsSendReady(0)){
            USART_Send(port_b_value,0);
        }            
        break;
        
        default:
        break;
    }
}

int main(void)
{
    DDRA=0x01;PORTA=0xFE;
    DDRB = 0xFF;PORTB = 0x00;
    initUSART(0);
    TimerSet(1000);
    TimerOn();
    
    state = Start;
    PORTA=0x01; //LEADER INDICATOR
    while(1)
    {
        Tick();
        
        while(!TimerFlag);
        TimerFlag = 0;
    }
}