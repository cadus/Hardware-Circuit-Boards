/*
 * TC2_8b_2ms.c
 *
 * Created: 07.03.2018 17:15:33
 *  Author: Moritz Klimt
 */ 

#include <avr/io.h>

void init_TC2_8b_2ms()
{
	PRR &= ~(1 << PRTIM2);				//Disable power reduction mode on TC2
	TCCR2A = 0x00;						//Set all bits in register TCCR2A to 0
	TCCR2B = 0x00;						//Set all bits in register TCCR2B to 0
	TCNT2  = 0x00;						//Set all bits in register TCNT2 to 0
	TCCR2A |=  (1<<WGM21);				//Set bits for Timer on Compare Match (CTC) OCR2A
	TCCR2B |=  (1<<CS22)|(1<<CS20);		//Set Clock Select bits Prescaler 128
	OCR2A   =   250-1;					//250 Timer Ticks to CTC
	TIMSK2 |=  (1<<OCIE2A);				//Output Compare A Match Interrupt Enable
}