/*
 * PULSOXYv0.0.1.c
 *
 * Created: 07.03.2018 15:49:20
 * Author : Moritz Klimt
 */ 

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <avr/interrupt.h>
#include <math.h>

extern "C" {
#include "ADC.h"
#include "TC2_8b_2ms.h"
#include "TC1_PWM_4kHz.h"
#include "UART.h"
#include "Update_Signals.h"
#include "LEDControl.h"
#include "AGC.h"
#include "calculatingSPO2.h"
}

volatile uint32_t GainDelay = 0;									//Variable for timing the Gain
volatile uint32_t *GainDelay_p = &GainDelay;
volatile uint32_t EnvelopeBufferTime = 0;							//Variable for timing the Envelope detection.
volatile uint32_t *EnvelopeBufferTime_p = &EnvelopeBufferTime;

uint8_t FingerDetection = 0;										//Variable set to 1 if a finger is detected and 0 if not					

volatile uint8_t BufferTimer = 0;									//Variable for setting the time intervals in which values for the SpO2 calculation are written to the buffer

volatile int16_t aACIRBuffer[125];									//Buffer for the IR-Values
volatile int16_t *aACIRBuffer_p = &aACIRBuffer[0];
volatile int16_t aACRBuffer[125];									//Buffer for the Red-Values
volatile int16_t *aACRBuffer_p = &aACRBuffer[0];
volatile uint8_t BufferDataPoint = 0;								//Variable that checks the level of the buffer

volatile uint8_t calculateSPO2 = 0;									//Variable is set to 1 when the buffer is full and a SpO2-value can be calculated

uint8_t SPO2 = 0;													//Variable for the calculated SpO2-value

int main(void)
{
	cli();															//Disable all Interrupts
	
    init_ADC();														//Initializes the ADC
	init_TC2_8b_2ms();												//Initializes the TC2 to trigger a timer interrupt every 2ms
	init_TC1_PWM_4kHz();											//Initializes the TC1 to generate a PWM with 12bit resolution and 4kHz, to control the red/IR LED brightness
	init_UART_115200_8N1();											//Initializes the serial communication via UART, with a baud rate of 115200
	
	sei();															//Enable all Interrupts
	
	init_AGC(GainDelay_p);											//Initializes the Auto Gain Control
	
	DDRD |= (1 << PD2);												//PIN PD2 Output This Pin is set to HIGH if the LED Brightness is getting adjusted
	
    while (1) 
    {
		FingerDetection = FingerIn_Out();							//Testing if the finger is in the Fingerclip or not
		
		if (FingerDetection==0)										//If no finger was detected, the measurement is reset
		{
			SendData(0);											//Send ERROR-Code 404 "No Finger Detected"
			PORTD |= (1 << PD2);									//Setting PIN PD2 to HIGH
			BufferDataPoint = 0;									//Reset the Buffer for the SpO2 calculation
			calculateSPO2 = 0;										
			for (uint8_t i = 0; i<10 ; i++)							//Reset the SpO2-Value and RAGC-Value Buffer
			{
				*(SPO2Values_p+i) = 0;
				*(RAGCValues_p+i) = 0;
			}
			SPO2DataPoints = 0;
			RAGCDataPoints = 0;
			if (RAGC > 1)											//If the AC gain is greater than 1, it is reset to 1
			{
				init_AGC(GainDelay_p);
			}
			for(uint32_t i = 0; i<1000000;i++)
			{
			}
		}
		
		if (FingerDetection==1)										//If a finger was detected, the SpO2 routine starts
		{
			PORTD &= ~(1 << PD2);									//Setting PIN PD2 to LOW
			
			//Control of the LED brightness
			if ((PPGIRDCsig >= 800)||(PPGRDCsig >= 800))			//If the LEDs are too bright, they are turned down
			{
				PORTD |= (1 << PD2);
				LEDBrightnessDOWN(GainDelay_p);
				BufferDataPoint = 0;
				calculateSPO2 = 0;
				for (uint8_t i = 0; i<10 ; i++)
				{
					*(RAGCValues_p+i) = 0;
				}
				RAGCDataPoints = 0;
			}
			if ((PPGRDCsig <= 400)||(PPGIRDCsig <= 400))			//If the LEDs are too dark, they are up-regulated
			{
				PORTD |= (1 << PD2);
				LEDBrightnessUP(GainDelay_p);
				BufferDataPoint = 0;
				calculateSPO2 = 0;
				for (uint8_t i = 0; i<10 ; i++)
				{
					*(RAGCValues_p+i) = 0;
				}
				RAGCDataPoints = 0;
			}
			
			//Adjusting the AC-Gain
				AGC(GainDelay_p,EnvelopeBufferTime_p);
			
			//Calculating SPO2			
			if ((calculateSPO2==1)&&(RAGCDataPoints > 1))
			{
				SPO2 = calculatingSPO2(aACIRBuffer_p,aACRBuffer_p,((uint16_t)(sizeof(aACIRBuffer)/sizeof(aACIRBuffer[0]))));
				SendData(SPO2);										//Transmission of the current SpO2 value
				calculateSPO2 = 0;
			}
			
		}
		

    }
}


//ISR which is called every 2ms
ISR(TIMER2_COMPA_vect)
{
	UpdateSignals();												//Detection of the required measuring signals
	GainDelay+=2;													//Counting the time for the Gain timing
	EnvelopeBufferTime+=2;											//Counting the time for the envelope detection timing
	BufferTimer++;													//Counting the ISR calls to write a data point onto the buffer every 20ms
	
	if(BufferTimer == 10)											//Write a data point onto the buffer every 20ms
	{
		BufferTimer = 0;
	
		if ((calculateSPO2 == 0))									//The buffer is only written as long as no new SpO2 value is calculated
		{
			*(aACIRBuffer_p+BufferDataPoint) = (int16_t)((PPGIRsig*10/PPGIRDCsig));
			*(aACRBuffer_p+BufferDataPoint) = (int16_t)((PPGRsig*10/PPGRDCsig));
			BufferDataPoint++;
		
			if(BufferDataPoint == ((uint16_t)(sizeof(aACIRBuffer)/sizeof(aACIRBuffer[0]))))		//When the buffer is full, the SpO2 value can be calculated
			{
				BufferDataPoint = 0;
				calculateSPO2 = 1;
			}
		}
	}
}

