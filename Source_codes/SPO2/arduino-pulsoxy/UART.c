/*
 * UART.c
 *
 * Created: 14.03.2018 12:21:11
 *  Author: Moritz Klimt
 */ 

#define F_CPU 16000000UL

#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <avr/interrupt.h>
//#include <util/delay.h>

void init_UART_115200_8N1()
{
	UCSR0A |= (1 << U2X0);							// Set the "Double Transmission Speed" bit in order to use a more precise timing mode for 115200 Baud
	UCSR0B |= ((1 << RXEN0) | (1 << TXEN0));		// Enable receive and transmit mode which again configure the respective pins accordingly
	UCSR0C |= ((1 << UCSZ01) | (1 << UCSZ00));		// Set 8-bit data mode
	UBRR0L = 207;									// Set BAUD rate to 115200;
}

void SendData(int16_t data)
{
	char data_string[6];
	char *data_string_p = &data_string[0];
	itoa(data, data_string_p, 10);
	while ((UCSR0A & (1 << UDRE0)) == 0);
	UDR0 = 'O';
	while ((UCSR0A & (1 << UDRE0)) == 0);
	UDR0 = 'X';
	for (uint8_t i=0; i < strlen(data_string); i++)
		{
			while ((UCSR0A & (1 << UDRE0)) == 0);
			UDR0 = *(data_string_p+i);
		}
	while ((UCSR0A & (1 << UDRE0)) == 0);
	UDR0 = 0x0A;
}

