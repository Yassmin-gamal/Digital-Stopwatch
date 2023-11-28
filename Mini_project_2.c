/*
 ***************************************************************************************************************************************
 Name        : Mini_project_2.c
 Author      : Yassmin Gamal
 Created on  : Apr 30, 2022
 Deadline    : 7/5/2022
 Description : Stop Watch system
 ***************************************************************************************************************************************
 */

#include<avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#define F_CPU 1000000
#define True 1


unsigned char hour = 0;
unsigned char minute = 0;
unsigned char second = 0;

//Resume the stop-watch
ISR(INT2_vect){
	//Resume the clock after Trigger INT2 with falling edge
	TCCR1B |= (1<<CS10) | (1<<CS12);
}

void INT2_Init_Resume(void){
	DDRB &= ~(1<<PB2);		// Configure INT0/PD2 as input pin
	PORTB |= (1<<PB2);		// Enable the internal pull up resistor at PB2 pin
	MCUCSR &= ~(1<<ISC2);	//Trigger INT2 with falling edge ISC2 = 0
	GICR |= (1<<INT2);		//External Interrupt 2 Enable => INT2 = 1
	SREG  |= (1<<7);		// Enable interrupts by setting I-bit
}


ISR(INT0_vect){
	//reset the variables
	hour = 0;
	minute = 0;
	second = 0;
}

//Reset the stop-watch
void INT0_Init_Reset(void){
	DDRD &= ~(1<<PD2);		// Configure INT0/PD2 as input pin
	PORTD |= (1<<PD2);		// Enable the internal pull up resistor at PD2 pin
	// Trigger INT0 with the falling edge
	MCUCR |= (1<<ISC01);	//ISC01 = 1, ISC00 = 0
	MCUCR &= ~(1<<ISC00);
	GICR |= (1<<INT0);		//External Interrupt 0 Enable => INT0 = 1
	SREG  |= (1<<7);		// Enable interrupts by setting I-bit
}


//pause the stop-watch
ISR(INT1_vect){
	//Stop the clock => CS12 = 0, CS11 = 0, CS10 = 0
	TCCR1B &= ~(1<<CS12) & ~(1<<CS11) & ~(1<<CS10);
}

void INT1_Init_Pause(void){
	DDRD &= ~(1<<PD3);	// Configure INT1/PD3 as input pin
	SREG |= (1<<7);		// Enable interrupts by setting I-bit
	// Trigger INT0 with the raising edge
	MCUCR |= (1<<ISC11) | (1<<ISC10);		//ISC11 = 1, ISC10 = 1
	GICR |= (1<<INT1);	//External Interrupt 1 Enable => INT1 = 1
}


ISR(TIMER1_COMPA_vect){
	second++;
	if(second == 60)
	{
		second -= 60;
		minute ++;
	}
	if(minute == 60)
	{
		minute -= 60;
		hour++;
	}
	if(hour == 24)
	{
		second =0;
		minute = 0;
		hour = 0;
	}

}

void CTC_Timer1_Init(void){
	// Set Timer initial value to 0
	TCNT1 = 0;
	//Set Compare Value
	OCR1A = 1000;
	/* Configure the timer mode
	 * 1. Non PWM mode FOC1A = 1
	 * 2. CTC Mode WGM10 = 0 & WGM11 = 0 & WGM12 = 1 & WGM13 = 0
	 * 3. clock = F_CPU/1024 CS10 = 1 CS11 = 0 CS12 = 1
	 */
	TCCR1A = (1<<FOC1A);
	TCCR1B = (1<<WGM12) | (1<<CS10) | (1<<CS12);
	SREG |= (1<<7);
	TIMSK |= (1<<OCIE1A);		//Timer1 enable compare match interrupt
}


int main(){

	DDRC |= 0x0F;	//First 4 pins in PORTC Output
	DDRA |= 0x3F;	//First 6 pins in PORTA Output
	PORTC = 0x00;	//Initially all LEDs of 7-segment are off
	PORTA = 0x3F;

	CTC_Timer1_Init();
	INT1_Init_Pause();
	INT0_Init_Reset();
	INT2_Init_Resume();
	while(True)
	{
		PORTA = (1<<PA0);
		PORTC = second % 10;
		_delay_us(50);
		PORTA = (1<<PA1);
		PORTC = second / 10;
		_delay_us(50);
		PORTA = (1<<PA2);
		PORTC = minute % 10;
		_delay_us(50);
		PORTA = (1<<PA3);
		PORTC = minute / 10;
		_delay_us(50);
		PORTA = (1<<PA4);
		PORTC = hour % 10;
		_delay_us(50);
		PORTA = (1<<PA5);
		PORTC = hour / 10;
		_delay_us(50);
	}

}
