/*
 * CS122A_Lab5_Part1_JOYSTICK.c
 *
 * Created: 24-10-2017 19:16:09
 * Author : Jeet
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include "C:\includes\io.c"

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


void A2D_init() {
    ADCSRA |= (1 << ADEN) | (1 << ADSC) | (1 << ADATE);
    // ADEN: Enables analog-to-digital conversion
    // ADSC: Starts analog-to-digital conversion
    // ADATE: Enables auto-triggering, allowing for constant
    //	    analog to digital conversions.
}
unsigned short input1=0x00;
unsigned char directionX=0x00;
unsigned short speedX=0x00;
unsigned char directionY=0x00;
unsigned short speedY=0x00;

signed char xpos=0;
signed char ypos=0;
enum SM1_States{Start1,checkJoystick_X}SM1_state;
void Tick1(){
    switch(SM1_state){
        case Start1:
            SM1_state=checkJoystick_X;
            break;
        case checkJoystick_X:
            SM1_state=checkJoystick_X;
            break;
        default:break;
    }   
    switch(SM1_state){
        case checkJoystick_X:
            input1=ADC;
            if(input1>550 && input1<787)
            {
                speedX=400;
                directionX=0x01;
            }
            else if(input1>787)
            {
                speedX=200;
                directionX=0x01;
            }
            else if(input1<500&&input1>250)
            {
                speedX=400;
                directionX=0x02;
            }
            else if(input1<250){
                speedX=200;
                directionX=0x02;
            }
            else
            {
                speedX=0;
                directionX=0x00;
            }
            ADMUX=1;
            break;
        default:break;
    } 
}

enum SM2_states{Start2,moveXaxis}SM2_state;
void Tick2(){
    switch(SM2_state){
        case Start2:
            SM2_state=moveXaxis;
            break;
        case moveXaxis:
            SM2_state=moveXaxis;
            break;
        default:break;
    }
    switch(SM2_state){
        case moveXaxis:
            if(directionX==0x01)
            {
                xpos++;
                if(xpos>7)
                    xpos=0;
            }
            else if(directionX==0x02)
            {
                xpos--;
                if(xpos<0)
                    xpos=7;
            }
            break;
        default:break;
    }
}

enum SM3_States{Start3,checkJoystick_Y}SM3_state;
void Tick3(){
    switch(SM3_state){
        case Start3:
        SM3_state=checkJoystick_Y;
        break;
        case checkJoystick_Y:
        SM3_state=checkJoystick_Y;
        break;
        default:break;
    }
    switch(SM3_state){
        case checkJoystick_Y:
        input1=ADC;
        if(input1>550 && input1<787)
        {
            speedY=400;
            directionY=0x01;
        }
        else if(input1>787)
        {
            speedY=200;
            directionY=0x01;
        }
        else if(input1<500&&input1>250)
        {
            speedY=400;
            directionY=0x02;
        }
        else if(input1<250){
            speedY=200;
            directionY=0x02;
        }
        else
        {
            speedY=0;
            directionY=0x00;
        }
        ADMUX=0;
        break;
        default:break;
    }
}

enum SM4_states{Start4,moveYaxis}SM4_state;
void Tick4(){
    switch(SM4_state){
        case Start4:
        SM4_state=moveYaxis;
        break;
        case moveYaxis:
        SM4_state=moveYaxis;
        break;
        default:break;
    }
    switch(SM4_state){
        case moveYaxis:
        if(directionY==0x01)
        {
            ypos--;
            if(ypos<0)
                ypos=7;
        }
        else if(directionY==0x02)
        {
            ypos++;
            if(ypos>7)
                ypos=0;
        }
        break;
        default:break;
    }
}

enum SM5_states{start5,displayMatrix}SM5_state;
void Tick5(){
    switch(SM5_state){
        case start5:
            SM5_state=displayMatrix;
            break;
        case displayMatrix:
            SM5_state=displayMatrix;
            break;
        default:break;
    }
    switch(SM5_state){
        case displayMatrix:
            switch(ypos){
                case 0:
                    PORTD=0xFE;
                    break;
                case 1:
                    PORTD=0xFD;
                    break;
                case 2:
                    PORTD=0xFB;
                    break;
                case 3:
                    PORTD=0xF7;
                    break;
                case 4:
                    PORTD=0xEF;
                    break;
                case 5:
                    PORTD=0xDF;
                    break;
                case 6:
                    PORTD=0xBF;
                    break;
                case 7:
                    PORTD=0x7F;
                    break;
            }
            PORTB=0x01<<(xpos);
            break;
        default: break;
    } 
}


int main(void)
{
    DDRB=0xFF;PORTB=0x00; 
    DDRC=0xFF;PORTC=0x00;
    DDRD=0xFF;PORTD=0x00;
    PORTD=0xFF;
    ADMUX=0;
    /* Replace with your application code */
      SM1_state=Start1;
      SM2_state=Start2;
      SM3_state=Start3;
      SM4_state=Start4;
      SM5_state=start5;
      TimerSet(10);
      TimerOn();
      A2D_init();
      //LCD_DisplayString(1,"Hello World!");    //LCD SCREEN CHECK
      unsigned short counterX=0x00;
      unsigned short counterY=0x00;
      unsigned short counter=0x00;
      while(1)
      {
         if(counter%2==0)
         {
             Tick1();
             if(counterX<speedX && speedX!=0){
                  counterX+=20;    
             }
             else{
                 Tick2();
                 counterX=0;
             }
         }
         else{                                       
             Tick3();
             if(counterY<speedY && speedY!=0){
                counterY+=20;
             }
             else{
                 Tick4();
                 Tick5();
                 counterY=0; 
             }
         }             
         
         counter++;
         if(counter==126)
            counter=0;       
         while(!TimerFlag);
         TimerFlag = 0;
      }
}

