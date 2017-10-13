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


enum States{Start, PORT_B_RECEIVE} state;
unsigned char port_b_value=0x00;
unsigned char counter_val=0x00;
void Tick(){
    switch (state){
        
        case Start:
        state=PORT_B_RECEIVE;
        break;
        
        case PORT_B_RECEIVE:
        state=PORT_B_RECEIVE;
        break;
    
        default:
        break;
    }
    
    switch (state){
        
        case PORT_B_RECEIVE:
        if(USART_HasReceived(0)){
            port_b_value=USART_Receive(0);
            USART_Flush(0);
            PORTB=port_b_value;
        }
        break;
        
        default:
        break;
    }
}

int main(void)
{
    DDRB = 0xFF;PORTB = 0x00;
    initUSART(0);
    USART_Flush(0);
    TimerSet(100);
    TimerOn();
    
    state = Start;
    while(1)
    {
        Tick();
        
        while(!TimerFlag);
        TimerFlag = 0;
    }
}