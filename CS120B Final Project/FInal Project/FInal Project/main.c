#include <avr/io.h>
#include <stdbool.h>
#include "io.c"
#include <avr/interrupt.h>

volatile unsigned char TimerFlag = 0; // TimerISR() sets this to 1. C programmer should clear to 0.
unsigned long _avr_timer_M = 1; // Start count from here, down to 0. Default 1 ms.
unsigned long _avr_timer_cntcurr = 0; // Current internal count of 1ms ticks
unsigned char tmpA = 0x00; 
unsigned char i = 0; 
const double D3 = 146.83; 
const double E3 = 164.81; 
const double G3 = 196; 
const double A3 = 220;
const double A3S = 233.08; 
const double B3 = 246.94; 
const double C4 = 261.63;
const double D = 293.66;
const double E4 = 329.63;
const double F = 349.23;
const double G = 392.00;
const double A = 440.00;
const double A4S = 466.16;
const double B = 493.88;
const double C5 = 523.25; 
const double D5 = 587.33; 
const double E5 = 659.25;
unsigned char p = 0;

void TimerOn() {
	TCCR1B = 0x0B;// bit3 = 0: CTC mode (clear timer on compare)
	
	OCR1A = 125;	// Timer interrupt will be generated when TCNT1==OCR1A
	TIMSK1 = 0x02; // bit1: OCIE1A -- enables compare match interrupt
	TCNT1=0;
	_avr_timer_cntcurr = _avr_timer_M;
	SREG |= 0x80; // 0x80: 1000000
}

void TimerOff() {
	TCCR1B = 0x00; // bit3bit1bit0=000: timer off
}

void TimerISR() {
	TimerFlag = 1;
}

ISR(TIMER1_COMPA_vect) {
	_avr_timer_cntcurr--; // Count down to 0 rather than up to TOP
	if (_avr_timer_cntcurr == 0) { // results in a more efficient compare
		TimerISR(); // Call the ISR that the user uses
		_avr_timer_cntcurr = _avr_timer_M;
	}
}

void TimerSet(unsigned long M) {
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}

void set_PWM(double frequency) {
	static double current_frequency;
	if (frequency != current_frequency) {
		if (!frequency) { TCCR3B &= 0x08; }
		else { TCCR3B |= 0x03; }
		if (frequency < 0.954) { OCR3A = 0xFFFF; }
		else if (frequency > 31250) { OCR3A = 0x0000; }
		else { OCR3A = (short)(8000000 / (128 * frequency)) - 1; }
		TCNT3 = 0;
		current_frequency = frequency;
	}
}

void PWM_on() {
	TCCR3A = (1 << COM3A0);
	TCCR3B = (1 << WGM32) | (1 << CS31) | (1 << CS30);
	set_PWM(0);
}

void PWM_off() {
	TCCR3A = 0x00;
	TCCR3B = 0x00;
}


void song1(){ //Jingle Bells
	double freq[] = {E4,E4,E4, E4,E4,E4, E4,G,C4,D, E4, F,F,F, F,F, 
					E4,E4,  E4,D,D,E4, D,G ,E4,E4,E4, E4,E4,E4, E4,G,C4,D, E4, F,F,F, F,F, E4,E4, G,G,F,D,C4};
	double beat[] = {300,300,400, 300,300,400, 300,300,300,300, 700, 300,300,300, 150,300,300,400, 150,300,300,300, 600,450, 
					300,300,400, 300,300,400, 300,300,300,300, 600, 300,300,300, 150,300,300,400, 300,300,300,300,700}; 
	double beatF[] = {150,150,200,150,150,200,150,150,150,150,350,150,150,150,75,150,150,200,75,150,150,150,300,225,
						300,300,400, 300,300,400, 300,300,300,300, 600, 300,300,300, 150,300,300,400, 300,300,300,300,700};
	i = 0; 
	for(; i<47; i++){
		if(p == 0){
			set_PWM(freq[i]);
			TimerSet(beat[i]);
 			while(!TimerFlag);
 			TimerFlag = 0;
		 
			//Blinking LEDS
			if(i%2 ==0)
				PORTB = 0x01;
			else
				PORTB = 0x02;
			set_PWM(0);
		}
		
		tmpA = ~PINA; 
		//Pause
		if(tmpA == 0x01){
			if(p == 0)
			p = 1;
			else
			p = 0;
		}
		if(p == 1){
			set_PWM(0);
			i--;
			while(!TimerFlag);
			TimerFlag = 0;
		}
		
		//Stop 
		if(tmpA == 0x02){
			set_PWM((0));
			break;
		}
		//Previous Song
		if(tmpA == 0x08)
			break; 
		//Next Song	
		if (tmpA == 0x10)
			break; 
	}
	
}
	
void song2(){   // Mario Theme
	double freq[] = {E4,E4,E4,C4,E4,G,100, C4,G3,E3,A, B,A4S,A,G, E4,G,A,F, G,E4,C4,D,B3}; 
	double beat[] = {250,250,300,250,300,500,600,  500,500,500,400, 300,300,300,400,  300,300,280,300, 280,350,260,250,550}; 
	i = 0;
	for(; i<24; i++){ 
		if(p == 0){
			set_PWM(freq[i]);
			TimerSet(beat[i]);
			while(!TimerFlag);
			TimerFlag = 0;
			
			//Blinking LEDS	
			if(i%2 ==0)
				PORTB = 0x01;
			else
				PORTB = 0x02;
			set_PWM(0);	 
		}
		
		tmpA = ~PINA; 
		
		//Pause
		if(tmpA == 0x01){
			if(p == 0)
				p = 1;
			else
				p = 0;
		}
		if(p == 1){
			set_PWM(0);
			i--;
			while(!TimerFlag);
			TimerFlag = 0;
		}
								
		//Stop 
		if(i>1){
			if(tmpA == 0x02){
				set_PWM((0));
				return;
			}
		}
		//Previous Song
		if(tmpA == 0x08)
			break; 
		//Next Song
		if(tmpA == 0x10)
			break; 
	}
}

void song3(){	//Happy Birthday
	double freq[]= {G,G,A,G,C5,B, G,G,A,G,D5,C5, G,G,783.99,E5,C5,B,440, 698.46,698.46,E5,C5,D5,C5};
	double beat[]= {200,200,300,300,300,400,200,200,300,300,300,400,200,200,300,300,300,300,300,200,200,300,300,300,400};
	i = 0; 
	for(; i<25; i++){
		if(p == 0){
			set_PWM(freq[i]);
			TimerSet(beat[i]);
			while(!TimerFlag);
			TimerFlag = 0;
	
			//Blinking LEDS
			if(i%2 ==0)
			PORTB = 0x01;
			else
			PORTB = 0x02;
			set_PWM(0);
		}
		
		tmpA = ~PINA; 
		//Pause
		if(tmpA == 0x01){
			if(p == 0)
			p = 1;
			else
			p = 0;
		}
		if(p == 1){
			set_PWM(0);
			i--;
			while(!TimerFlag);
			TimerFlag = 0;
		}
		//Stop 
		if(tmpA == 0x02){
			set_PWM((0));
			break;
		}
		//Previous Song
		if(tmpA == 0x08)
			break;
		//Next Song
		if(tmpA == 0x10)
			break;
	}
}
enum LCD_States{init, menu, songList, songList2, dispSong1, dispSong2, dispSong3}LCD_State;

void LCD_FSM(){
	tmpA = ~PINA;
	switch(LCD_State){//transitions
		case init:
			LCD_State = menu;
			break;
		
		case menu:
			if(tmpA)
				LCD_State = songList;
			else
				LCD_State = menu;
			break;
		
		case songList:
			if(tmpA == 0x01)
				LCD_State = dispSong1;
			else if(tmpA == 0x02)
				LCD_State = dispSong2;
			else if(tmpA == 0x10)
				LCD_State = songList2;    
			else
				LCD_State = songList;
			break;
		
		case songList2: 
			if(tmpA == 0x08)
				LCD_State = songList; 
			else if(tmpA == 0x04)
				LCD_State = dispSong3; 
		case dispSong1:	
			if(0x00)
				LCD_State = menu;
			else if(0x08)
			
			break;
		
		case dispSong2:	
			if(0x00)
				LCD_State = menu;
			break;
		
		case dispSong3: 
			if(0x00)
				LCD_State = menu;
			break; 
			
		default:
			LCD_State = init;
			break;
	}//transition
	
	switch(LCD_State){//state actions
		case init:
			break;
			
		case menu:
			LCD_DisplayString(1, "    JUKEBOX     Press any button");
			break;
			
		case songList:
			LCD_DisplayString(1, "1 Jingle Bells  2 Mario Theme");
			break;
			
		case songList2: 
			LCD_DisplayString(1, "3 Happy Birthday");
			break; 
			
		case dispSong1:
			LCD_DisplayString(1, "1 Jingle Bells");
			song1();
			if(tmpA == 0x08) //Previous 
				LCD_State = dispSong3; 
			else if (tmpA == 0x10) //Next 
				LCD_State = dispSong2; 
			else 
				LCD_State = menu;
			break;
			
		case dispSong2:
			LCD_DisplayString(1, "2  Mario Theme");
			song2(); 
		    if(tmpA == 0x08)
				 LCD_State = dispSong1;
		    else if (tmpA == 0x10)
				  LCD_State = dispSong3;
		    else
				 LCD_State = menu;
			break;
			
		case dispSong3: 
			LCD_DisplayString(1, "3 Happy Birthday");
			song3(); 
			if(tmpA == 0x08)
				LCD_State = dispSong2;
			else if (tmpA == 0x10)
				LCD_State = dispSong1;
			else
				LCD_State = menu;
			break; 
			
		default: 
			break; 
	}//state actions
}

int main(void)
{	
	DDRA = 0x00; PORTA = 0xFF; 
	DDRB = 0xFF; PORTB = 0x00; 
	DDRC = 0xFF; PORTC = 0x00; // LCD data lines
	DDRD = 0xFF; PORTD = 0x00; // LCD control lines
	
	LCD_init();
	TimerSet(250); 
	TimerOn(); 
	PWM_on(); 
	LCD_State = init;
	
    while (1) 
    {
		LCD_FSM();
		while(!TimerFlag);
		TimerFlag = 0;
    }
}

