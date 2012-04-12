#include <avr/io.h>
#include <avr/interrupt.h>
#include <inttypes.h>

#define PIN_UP   (PORTB |=  (1<<3));
#define PIN_DOWN (PORTB &= ~(1<<3));

#define RISING_EDGE_INTERRUPT  (MCUCR=3);
#define FALLING_EDGE_INTERRUPT (MCUCR=2);

//global variables
volatile uint16_t time;
volatile uint16_t last;
volatile uint16_t up_time;

ISR(INT0_vect)
{
		if(MCUCR&0x3) {
			up_time = time;
			FALLING_EDGE_INTERRUPT;
		}else
		{
			if(time > up_time) {
				last=time - up_time-200;
			}else{
				last = up_time - time-220;
			}
			RISING_EDGE_INTERRUPT;
		}
				
}

ISR(TIM0_COMPA_vect)
{
	//Timer interrupt will be generated every 5us
	time++;
	
}

void init_timer()
{
	TCCR0A = 0;
	TCCR0B = (1<<CS01);
	OCR0A = 60;
	TIMSK0=(2<<1); //Enable compare match 0 interrupt
	TCNT0=0;
	
	
}

void init_ext_int()
{
		RISING_EDGE_INTERRUPT;
		GIMSK = (1<<6);
}

int main()
{
	init_timer();
	init_ext_int();
	PORTB=(1<<3);
	time=last=up_time=0;
	sei();
	while(1) { 
		if(last>=36) {
			if(last<=(36+128)) {
				PIN_UP;
			} else { 
				if( time%(~((last-36)<<2)) == 0) { 
					PIN_UP; 
				}else{ 
					PIN_DOWN; 
				}
			} 
		}else { 
				PIN_DOWN; 
		}
	} 
	
}
