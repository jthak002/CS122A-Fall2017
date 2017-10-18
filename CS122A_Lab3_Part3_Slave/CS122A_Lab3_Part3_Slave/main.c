/*
 * cs122a_lab3_part1_slave.c
 *
 * Created: 10/12/2017 5:02:24 PM
 * Author : edward
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>

volatile unsigned char TimerFlag = 0;
unsigned long avr_timer_M = 1;
unsigned long avr_timer_cntcurr = 0;

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

//SPI
//Slave code
unsigned char receivedData = 0x00;

#define DDR_SPI DDRB
#define DD_MOSI 5
#define DD_MISO 6
#define DD_SCK 7
#define DD_SS 4
#define PORT_SPI PORTB

void SPI_SlaveInit(void){
    DDRB = (1 << 6);
    SPCR = (1<<SPE) | (1<<SPIE);
}

unsigned char SPI_Transmit(unsigned char data){
    SPDR = data;
    while( !(SPSR & (1<<SPIF) ));
    return SPDR;
}

ISR(SPI_STC_vect){
    receivedData = SPDR;
    
};

enum States{Start} state;
unsigned char testvar = 'u';
unsigned char portd_var = 0x00;
unsigned char testChar = 0x33;
unsigned char readKey = '\0';
unsigned char period_length=40;
unsigned char counter=0x00;
unsigned char pattern_mode=1;
unsigned char time_mode=1;
void Tick(){
	switch (state){	//Transitions
		
		case Start:
		break;
		
		default:
		break;
		
	}
	
	switch (state){	//State Actions
		
		case Start:
			//receivedData = SPI_Transmit(testChar);
			pattern_mode=receivedData&0x0F;
            time_mode=(receivedData&0xF0)>>4;
            if(time_mode==1)
                period_length=40;
            else if(time_mode==2)
                period_length=20;
            else if(time_mode==3)
                period_length=10;
            else if(time_mode==4)
                period_length=5;
            else if(time_mode==5)
                period_length=2;
            else if(time_mode==6)
                period_length=1;
		break;
		
		default:
		break;
	}
}

enum sm_pattern1{start1,state_11,state_12} sm1;
void Tick_SM1(){
    switch(sm1){
        case start1:
            sm1=state_11;
            break;
        case state_11:
            if(counter<period_length)
                counter++;
            else{
                counter=0;
                sm1=state_12;
            }
            break;
        case state_12:
            if(counter<period_length)
                counter++;
            else{
                counter=0;
                sm1=state_11;
            }
            break;
        default:break;
    }
    switch(sm1){
        case state_11:
            PORTD=0xF0;
            break;
        case state_12:
            PORTD=0x0F;
            break;
        default:break;
    }
}

enum sm_pattern2{start2,state_21,state_22} sm2;
void Tick_SM2(){
    switch(sm2){
        case start2:
        sm2=state_21;
        break;
        case state_21:
        if(counter<period_length)
        counter++;
        else{
            counter=0;
            sm2=state_22;
        }
        break;
        case state_22:
        if(counter<period_length)
        counter++;
        else{
            counter=0;
            sm2=state_21;
        }
        break;
        default:break;
    }
    switch(sm2){
        case state_21:
        PORTD=0xAA;
        break;
        case state_22:
        PORTD=0x66;
        break;
        default:break;
    }
} 
enum sm_pattern3{start3,state_31,state_32} sm3;
void Tick_SM3(){
    switch(sm3){
        case start3:
        sm3=state_31;
        break;
        case state_31:
        if(counter<period_length)
        counter++;
        else{
            counter=0;
            sm3=state_32;
        }
        break;
        case state_32:
        if(counter<period_length)
        counter++;
        else{
            counter=0;
            sm3=state_31;
        }
        break;
        default:break;
    }
    switch(sm3){
        case state_31:
        PORTD=0xAA;
        break;
        case state_32:
        PORTD=0x55;
        break;
        default:break;
    }
}
enum sm_pattern4{start4,state_41,state_42,state_43,state_44,state_45,state_46,state_47,state_48} sm4;
void Tick_SM4(){
    switch(sm4){
        case start4:
        sm4=state_41;
        break;
        
        case state_41:
        if(counter<period_length)
        counter++;
        else{
            counter=0;
            sm4=state_42;
        }
        break;
        
        case state_42:
        if(counter<period_length)
        counter++;
        else{
            counter=0;
            sm4=state_43;
        }
        break;
                case state_43:
                if(counter<period_length)
                counter++;
                else{
                    counter=0;
                    sm4=state_44;
                }
                break;
                        case state_44:
                        if(counter<period_length)
                        counter++;
                        else{
                            counter=0;
                            sm4=state_45;
                        }
                        break;
                                case state_45:
                                if(counter<period_length)
                                counter++;
                                else{
                                    counter=0;
                                    sm4=state_46;
                                }
                                break;
                                case state_46:
                                if(counter<period_length)
                                counter++;
                                else{
                                    counter=0;
                                    sm4=state_47;
                                }
                                break;
                                        case state_47:
                                        if(counter<period_length)
                                        counter++;
                                        else{
                                            counter=0;
                                            sm4=state_48;
                                        }
                                        break;
                                                case state_48:
                                                if(counter<period_length)
                                                counter++;
                                                else{
                                                    counter=0;
                                                    sm4=state_41;
                                                }
                                                break;
        default:break;
    }
    switch(sm4){
        case state_41:
        PORTD=0x01;
        break;
        case state_42:
        PORTD=0x02;
        break;
        case state_43:
        PORTD=0x04;
        break;
        case state_44:
        PORTD=0x08;
        break;
        case state_45:
        PORTD=0x10;
        break;
        case state_46:
        PORTD=0x20;
        break;
        case state_47:
        PORTD=0x40;
        break;
        case state_48:
        PORTD=0x80;
        break;
        default:break;
    }
}

int main(void)
{
	DDRD=0xFF;PORTD=0x00;
	DDRB=0x00;PORTB=0xFF;
	
	TimerSet(50);
	TimerOn();
	
	state = Start;
	sm1=start1;
    sm2=start2;
    sm3=start3;
    sm4=start4;
	SPI_SlaveInit();
	
	while(1)
	{
		Tick();
        
		switch(pattern_mode){
            case 1:
                Tick_SM1();
                break;
            case 2:
                Tick_SM3();
                break;
            case 3:
                Tick_SM4();
                break;
            case 4:
                Tick_SM2();
                break;
             default:break;
        }
		while(!TimerFlag);
		TimerFlag = 0;
	}
}

