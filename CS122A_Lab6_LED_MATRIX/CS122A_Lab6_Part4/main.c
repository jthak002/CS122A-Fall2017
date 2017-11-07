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

unsigned char ledMatrix[8][8];
unsigned char indexX=0x03;
unsigned char indexY=0x03;
unsigned char directionX=0x00;
unsigned char directionY=0x00;
unsigned char input=0x00;
enum SM1_states{Start1, checkButtonPressa0, checkButtonLifta0}SM1_state;
void TickSM1(){
    input=(~PINA)&0x01;
    switch(SM1_state){
        case Start1:
            SM1_state=checkButtonPressa0;
            break;
        case checkButtonPressa0:
            if(input>0){
                SM1_state=checkButtonLifta0;
                directionX=0x01;
            }                
             else
             {
                SM1_state=checkButtonPressa0;
                directionX=0x00;
             }                
            break;
        case checkButtonLifta0:
            if(input>0)
            {
                SM1_state=checkButtonLifta0;
                directionX=0x00;
            }            
            else
            {
                SM1_state=checkButtonPressa0;
                directionX=0x00;
            }            
            break;
        default:break;
    }
}
enum SM2_states{Start2, checkButtonPressa2, checkButtonLifta2}SM2_state;
void TickSM2(){
    input=(~PINA)&0x04;
    switch(SM2_state){
        case Start2:
        SM2_state=checkButtonPressa2;
        break;
        case checkButtonPressa2:
        if(input>0)
        {
            SM2_state=checkButtonLifta2;
            directionY=0x01;
        }        
        else
        {
            SM2_state=checkButtonPressa2;
            directionY=0x00;
        }        
        break;
        case checkButtonLifta2:
        if(input>0)
        {
            SM2_state=checkButtonLifta2;
            directionY=0x00;
        }        
        else{
            SM2_state=checkButtonPressa2;
            directionY=0x00;
        }
        break;
        default:break;
    }
}
enum SM3_states{Start3,shiftLEDmatrix}SM3_state;
void TickSM3(){
    switch (SM3_state)
    {
        case Start3:
            SM3_state=shiftLEDmatrix;
            break;
        case shiftLEDmatrix:
            SM3_state=shiftLEDmatrix;
            break;
        default:break;
    }
    switch(SM3_state){
        case shiftLEDmatrix:
            if(directionX==0x01)
            {
                if(indexX>0x01)
                   indexX--;
            }
            else if(directionX==0x02)
            {
                 if(indexX<0x06)
                   indexX++;
            }
            
            if(directionY==0x01)
            {
                 if(indexY<0x06)
                 indexY++;

            }
            else if(directionY==0x02)
            {
                if(indexY>0x01)
                indexY--;              
            }
            break;
        default:break;
    }
}

enum SM4_states{Start4,checkButtonPressa1, checkButtonLifta1} SM4_state;
void TickSM4(){
    input=(~PINA)&0x02;
    switch(SM4_state)
    {
        case Start4:
            SM4_state=checkButtonPressa1;
            break;
        case checkButtonPressa1:
            if(input>0)
            {
                SM4_state=checkButtonLifta1;
                directionX=0x02;
            }
            else
            {
                SM4_state=checkButtonPressa1;
                directionX=0x00;
            }
            break;
        case checkButtonLifta1:
            if(input>0)
            {
                SM4_state=checkButtonLifta1;
                directionX=0x00;
            }
            else
            {
                SM4_state=checkButtonPressa1;
                directionX=0x00;
            }
        break;
        default:break;        
    }
}

enum SM5_states{Start5, checkButtonPressa3, checkButtonLifta3} SM5_state;
void TickSM5(){
    input=(~PINA)&0x08;
    switch(SM5_state)
    {
        case Start5:
            SM5_state=checkButtonPressa3;
            break;
        case checkButtonPressa3:
            if(input>0)
            {
                SM5_state=checkButtonLifta3;
                directionY=0x02;
            }
            else
            {
                SM5_state=checkButtonPressa3;
                directionY=0x00;
            }
            break;
        case checkButtonLifta3:
            if(input>0)
            {
                SM5_state=checkButtonLifta3;
                directionY=0x00;
            }
            else
            {
                SM5_state=checkButtonPressa3;
                directionY=0x00;
            }
        break;
        default:break;
    }   
}

unsigned char displayCell(unsigned char cellCounter)
{
    unsigned char outputD=0XFF;;
    for(int i=0;i<8;i++)
    {
        if(ledMatrix[cellCounter/8][i]==1)
        {
            outputD=outputD&(~(0x01<<i));
        }
    }
    return outputD;
}
unsigned char cellCounter=0x00;    
enum SM6_states{Start6, refreshMatrix, displayMatrix}SM6_state;
void TickSM6()
{
    switch(SM6_state){
        case Start6:
            SM6_state=refreshMatrix;
            break;
        case refreshMatrix:
            SM6_state=displayMatrix;
            break;
        case displayMatrix:
            if(cellCounter==64)
                SM6_state=refreshMatrix;
            else
                SM6_state=displayMatrix;
            break;
        default:break;
    }
    switch(SM6_state){
        case refreshMatrix:
            for(int i=0;i<8;i++)
            {
                for(int j=0;j<8;j++)
                    ledMatrix[i][j]=0;
            }
            ledMatrix[indexX-1][indexY-1]=1;
            ledMatrix[indexX-1][indexY]=1;
            ledMatrix[indexX-1][indexY+1]=1;
            ledMatrix[indexX][indexY-1]=1;
            ledMatrix[indexX][indexY+1]=1;
            ledMatrix[indexX+1][indexY-1]=1;
            ledMatrix[indexX+1][indexY]=1;
            ledMatrix[indexX+1][indexY+1]=1;
        break;
        case displayMatrix:
            PORTD=0xFF;
            if(cellCounter==64)
                cellCounter=0;
            PORTB=(0x01<<(cellCounter/8));
            PORTD=displayCell(cellCounter);
            cellCounter+=8;
            break;
        default:
        break;                               
    }
}
int main(void)
{
    DDRA=0x00; PORTA=0xFF;
    DDRB=0xFF;PORTB=0x00; 
    DDRC=0xFF;PORTC=0x00;
    DDRD=0xFF;PORTD=0x00;
    PORTD=0xFF;
    /* Replace with your application code */
      TimerSet(1);
      TimerOn();
      //LCD_DisplayString(1,"Hello World!");    //LCD SCREEN CHECK
      SM1_state=Start1;
      SM2_state=Start2;
      SM3_state=Start3;
      SM4_state=Start4;
      SM5_state=Start5;
      SM6_state=Start6;
      unsigned char counter=0x00;
      while(1)
      {
         if(counter==100)
         {
            TickSM1();
            TickSM2();
            TickSM3();
            TickSM4();
            TickSM5();
            TickSM3();
            counter=0x00;
         }
         else
         {         
         TickSM6();
         counter++;
         }         
         while(!TimerFlag);
         TimerFlag = 0;
      }
}

