#include <avr/io.h>
#include <avr/interrupt.h>
#include "C:\includes\io.c"
#include "C:\includes\io.h"
#include "C:\includes\Queue.h"

//**********TIMER_CODE********************
volatile unsigned char TimerFlag = 0;
unsigned long avr_timer_M = 1;
unsigned long avr_timer_cntcurr = 0;
void TimerISR(){TimerFlag=1;}
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
//**********END_OF_TIMER_CODE************

//****************KEYPAD CODE***************************
unsigned char SetBit(unsigned char x, unsigned char k, unsigned char b) {
	return (b ? x | (0x01 << k) : x & ~(0x01 << k));
}
unsigned char GetBit(unsigned char x, unsigned char k) {
	return ((x & (0x01 << k)) != 0);
}
#define KEYPADPORT PORTA
#define KEYPADPIN  PINA
#define ROW1 0
#define ROW2 1
#define ROW3 2
#define ROW4 3
#define COL1 4
#define COL2 5
#define COL3 6
#define COL4 7

////////////////////////////////////////////////////////////////////////////////
//Functionality - Gets input from a keypad via time-multiplexing
//Parameter: None
//Returns: A keypad button press else '\0'
unsigned char GetKeypadKey() {

	// Check keys in col 1
	KEYPADPORT = SetBit(0xFF,COL1,0); // Set Px4 to 0; others 1
	asm("nop"); // add a delay to allow PORTx to stabilize before checking
	if ( GetBit(~KEYPADPIN,ROW1) ) { return '1'; }
	if ( GetBit(~KEYPADPIN,ROW2) ) { return '4'; }
	if ( GetBit(~KEYPADPIN,ROW3) ) { return '7'; }
	if ( GetBit(~KEYPADPIN,ROW4) ) { return '*'; }

	// Check keys in col 2
	KEYPADPORT = SetBit(0xFF,COL2,0); // Set Px5 to 0; others 1
	asm("nop"); // add a delay to allow PORTx to stabilize before checking
	if ( GetBit(~KEYPADPIN,ROW1) ) { return '2'; }
	if ( GetBit(~KEYPADPIN,ROW2) ) { return '5'; }
	if ( GetBit(~KEYPADPIN,ROW3) ) { return '8'; }
	if ( GetBit(~KEYPADPIN,ROW4) ) { return '0'; }

	// Check keys in col 3
	KEYPADPORT = SetBit(0xFF,COL3,0); // Set Px6 to 0; others 1
	asm("nop"); // add a delay to allow PORTx to stabilize before checking
	if ( GetBit(~KEYPADPIN,ROW1) ) { return '3'; }
	if ( GetBit(~KEYPADPIN,ROW2) ) { return '6'; }
	if ( GetBit(~KEYPADPIN,ROW3) ) { return '9'; }
	if ( GetBit(~KEYPADPIN,ROW4) ) { return '#'; }

	// Check keys in col 4
	KEYPADPORT = SetBit(0xFF,COL4,0); // Set Px7 to 0; others 1
	asm("nop"); // add a delay to allow PORTx to stabilize before checking
	if (GetBit(~KEYPADPIN,ROW1) ) { return 'A'; }
	if (GetBit(~KEYPADPIN,ROW2) ) { return 'B'; }
	if (GetBit(~KEYPADPIN,ROW3) ) { return 'C'; }
	if (GetBit(~KEYPADPIN,ROW4) ) { return 'D'; }
	
	return '\0';
}
//*****************END_KEY_PAD_CODE***************************************

//SPI
// Master code

void SPI_MasterInit(void) {
    DDRB = (1<<DDB4) | (1<<DDB5) | (1<<DDB7);
    SPCR = (1<<SPE) |  (1<<MSTR) | (1<SPR0);
}

unsigned char SPI_Transmit(unsigned char data){
    PORTB = PORTB & 0xEF;
    SPDR = data;
    while( !(SPSR & (1<<SPIF) ));
    PORTB = PORTB | 0x10;
    return SPDR;
}

unsigned char get_val=0x00;
unsigned char pattern_num=0x01;
unsigned char speed_num=0x01;
unsigned char prev_pattern_num=0x01;
unsigned char prev_speed_num=0x01;
unsigned char send_value=0x00;
unsigned char prev_send_val=0x11;
enum SM3_States{Start3, Keypad1} state_SM3;
void Tick_SM3()
{
    switch(state_SM3){
        case Start3:
            state_SM3=Keypad1;
            break;
        case Keypad1:
            state_SM3=Keypad1;
            break;
        default:break;
    }
    switch(state_SM3){
        case Keypad1:
            get_val=GetKeypadKey();
            if(get_val=='1'){
                speed_num=0x01;               
            }
            else if(get_val=='2'){
                speed_num=0x02;                
            }
            else if(get_val=='3'){
                speed_num=0x03;                
            }
            else if(get_val=='4'){
                speed_num=0x04;                
            }
            else if(get_val=='5'){
                speed_num=0x05;                
            }
            else if(get_val=='6'){
                speed_num=0x06;               
            }            
            else if(get_val=='A'){
                pattern_num=0x01;                
            }
            else if(get_val=='B'){
                pattern_num=0x02;
            }
            else if(get_val=='C'){
                pattern_num=0x03;
            }
            else if(get_val=='D'){
                pattern_num=0x04;
            }
            break;
        default:break;
    }
}
//STATE MACHINE TO OUTPUT ON LCD DISPLAY
enum SM2_States{Start2, WRITE_DISPLAY} state_SM2;
void Tick_SM2(){
    switch(state_SM2){
        case Start2:
            state_SM2=WRITE_DISPLAY;
            break;
        case WRITE_DISPLAY:
            state_SM2=WRITE_DISPLAY;
            break;
         default:
         break;
    }
    switch(state_SM2)
    {
        case WRITE_DISPLAY:
            if(prev_pattern_num!=pattern_num){
                if(pattern_num==0x01)
                {
                    LCD_Cursor(6);
                    LCD_WriteData('1');
                    LCD_Cursor(16);
                }
                else if(pattern_num==0x02)
                {
                    LCD_Cursor(6);
                    LCD_WriteData('2');
                    LCD_Cursor(16);
                }
                else if(pattern_num==0x03)
                {
                    LCD_Cursor(6);
                    LCD_WriteData('3');
                    LCD_Cursor(16);
                }
                else if(pattern_num==0x04)
                {
                    LCD_Cursor(6);
                    LCD_WriteData('4');
                    LCD_Cursor(16);
                }
                prev_pattern_num=pattern_num;
                send_value=(0x00|(speed_num<<4)|(pattern_num));
            }
            if(prev_speed_num!=speed_num){
                if(speed_num==0x01)
                {
                    LCD_Cursor(12);
                    LCD_WriteData('1');
                    LCD_Cursor(16);
                }
                else if(speed_num==0x02)
                {
                    LCD_Cursor(12);
                    LCD_WriteData('2');
                    LCD_Cursor(16);
                }
                else if(speed_num==0x03)
                {
                    LCD_Cursor(12);
                    LCD_WriteData('3');
                    LCD_Cursor(16);
                }
                else if(speed_num==0x04)
                {
                    LCD_Cursor(12);
                    LCD_WriteData('4');
                    LCD_Cursor(16);
                }
                else if(speed_num==0x05)
                {
                    LCD_Cursor(12);
                    LCD_WriteData('5');
                    LCD_Cursor(16);
                }
                else if(speed_num==0x06)
                {
                    LCD_Cursor(12);
                    LCD_WriteData('6');
                    LCD_Cursor(16);
                }                
                prev_speed_num=speed_num;
                send_value=(0x00|(speed_num<<4)|(pattern_num));
            }
            break;
            default:break;
    }
}
enum SM4_States {Start4, SPI_SEND} State_SM4;
void Tick_SM4(){
    switch (State_SM4)
    {
        case Start4:
            State_SM4=SPI_SEND;
            break;
        case SPI_SEND:
            State_SM4=SPI_SEND;
            break;
            default:break;
    }
    switch(State_SM4){
        case SPI_SEND:
            if(send_value!=prev_send_val){
                SPI_Transmit(send_value);
                prev_send_val=send_value;
            }
            break;
        default:break;
    }
}
int main(void)
{
	DDRA = 0xF0;
	PORTA = 0x0F;
    DDRB=0xFF; PORTB=0x00;
    DDRC=0xFF;PORTC=0x00;
    DDRD=0xFF; PORTD=0x00;
    
    LCD_init();
    LCD_DisplayString(1,"Ptrn:1 Spd:1");
    SPI_MasterInit();
    //LCD_DisplayString(17,"uC:1");
    TimerSet(100);
    TimerOn();
    
    state_SM3 = Start3;
    state_SM2 = Start2;
    State_SM4=Start4;
    while(1)
    {
        Tick_SM3();
        Tick_SM2();
        Tick_SM4();
        while(!TimerFlag);
        TimerFlag = 0;
    }
}