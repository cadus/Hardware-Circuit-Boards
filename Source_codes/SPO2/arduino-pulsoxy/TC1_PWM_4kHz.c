/*
 * TC1_PWM_4kHz.c
 *
 * Created: 13.03.2018 11:53:10
 *  Author: Moritz Klimt
 */ 

#include <avr/io.h>

void init_TC1_PWM_4kHz()
{
	PRR &= ~(1 << PRTIM1);				//Disable power reduction mode on TC1
	DDRB |= (1 << PB1);					//PIN PB1 Output
	DDRB |= (1 << PB2);					//PIN PB2 Output
	TCCR1A = 0x00;						//Set all bits in register TCCR1A to 0
	TCCR1B = 0x00;						//Set all bits in register TCCR1B to 0
	TCCR1C = 0x00;						//Set all bits in register TCCR1C to 0
	TCCR1A |= (1<<COM1A1)|(1<<COM1B1);	//Clear OC1A/OC1B on Compare Match, set OC1A/OC1B at BOTTOM (non-inverting mode)
	TCCR1A |= (1<<WGM11);				//Set TC1 Mode to Fast PWM
	TCCR1B |= (1<<WGM12)|(1<<WGM13);	//Set TC1 Mode to Fast PWM
	TCCR1B |= (1<<CS10);				//Set Clock Select Bit to Prescaler 1
	ICR1 = 4095;						//Set The Input Capture Register to 4095 for a 12-Bit resolution of the PWM
	OCR1A = 675;						//Set PWM-Duty-Cycle to 16%
	OCR1B = 675;						//Set PWM-Duty-Cycle to 16%
	
}