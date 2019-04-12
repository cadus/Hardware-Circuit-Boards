/*
 * ADC.c
 *
 * Created: 07.03.2018 16:02:45
 *  Author: Moritz Klimt
 */ 

#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>



void init_ADC()
{
	PRR &= ~(1 << PRADC);					//Switch off power reduction mode of the ADC (required for enabling the ADC)
	ADCSRA  = 0x00;							//Set all bits in register ADCSRA to 0
	ADCSRB  = 0x00;							//Set all bits in register ADCSRB to 0
	ADMUX	= 0x00;							//Set all bits in register ADMUX to 0
	ADMUX  |= (1 << REFS0);					//Set AVcc as voltage Reference
	ADMUX  |= (1 << ADLAR);					//Set ADC Left Adjust Result
	ADCSRA |= (1 << ADPS2)|(1 << ADPS1);	//Set ADC prescaler to 64
	ADCSRA |= (1 << ADEN);					//Enable ADC
}

uint16_t ADCread(uint8_t adcpin)
{
	volatile uint8_t ADCLval = 0;			//variable to read the ADCL bits
	volatile uint8_t ADCHval = 0;			//variable to read the ADCH bits
	ADMUX  &= (0xF0);						//Set the Input Channel Selection bits to 0
	ADMUX  |= (adcpin & 0x0F);				//Select the ADC Input Channel
	ADCSRA |= (1 << ADSC);					//Start ADC Conversion
	while(!(ADCSRA & (1 << ADIF)));			//Wait until the ADC Conversion is Complete
	ADCLval = ADCL;							//Read the ADCL register
	ADCHval = ADCH;							//Read the ADCH register
	return (ADCHval<<2)|(ADCLval>>6);		//Return the 10bit ADC value
	ADCSRA |= (1 << ADIF);					//Clear ADC Interrupt Flag
}