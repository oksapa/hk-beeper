#include <avr/io.h>
#include <avr/interrupt.h>
#include <inttypes.h>

#define F_CPU 9600000UL  // 9.6 MHz
#include <util/delay.h>

#define PIN_UP   (PORTB |=  (1<<3));
#define PIN_DOWN (PORTB &= ~(1<<3));

#define RISING_EDGE_INTERRUPT  (MCUCR=3);
#define FALLING_EDGE_INTERRUPT (MCUCR=2);

//global variables
volatile uint16_t time;
volatile uint16_t last;
volatile uint16_t up_time;
volatile uint8_t state;
ISR(INT0_vect)
{
		if(state) {
			up_time = time;
			FALLING_EDGE_INTERRUPT;
			state=0;
		}else
		{
			//PORTB=~PORTB;
			if(time > up_time) {
				last= time - up_time - 200-36; 
			}else{
				last = up_time - time - 200 -36;
			}
			RISING_EDGE_INTERRUPT;
			state=1;
		}
				
}

ISR(TIM0_COMPA_vect)
{
	//PORTB=~PORTB;
	//Timer interrupt will be generated every 5us
	time++;
}

void init_timer()
{
	TCCR0A = (1<<WGM01);
	TCCR0B = (1<<CS00);
	OCR0A = 48;
	TIMSK0=(2<<1); //Enable compare match 0 interrupt
	TCNT0=0;
	
	
}

void init_ext_int()
{
		RISING_EDGE_INTERRUPT;
		state=1;
		GIMSK = (1<<6);
}

void delay(uint8_t t)
{
	for(uint8_t i=1; i<t;i++)
	{ 
		_delay_us(1);
	}
}

int main()
{
	init_timer();
	init_ext_int();
	DDRB=(1<<3);
	time=last=up_time=0;
	sei();

	while(1) { 
		if(last > 0) {
			if(last > 128 ) {
				if (last < 300) {
					PIN_UP;
				}
			} else { 
				PIN_UP;
				delay(last);
				PIN_DOWN;
				delay(128-last);	 
			} 
		}else { 
				PIN_DOWN; 
		}
	} 
	
}
