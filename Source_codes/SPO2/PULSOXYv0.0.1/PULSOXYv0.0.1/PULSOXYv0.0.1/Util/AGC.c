/*
 * AGC.c
 *
 * Created: 04.04.2018 15:22:19
 *  Author: Moritz Klimt
 */ 

#include <avr/io.h>
#include <stdint.h>
#include "Update_Signals.h"

#define INC PD7
#define UD PB0

volatile int32_t PPGIRACDCmax = -1023;
volatile int32_t PPGIRACDCmin = 1023;
volatile int32_t PPGRACDCmax = -1023;
volatile int32_t PPGRACDCmin = 1023;
volatile uint32_t PPGpeakpeak = 0;
uint8_t RAGC = 0;
uint8_t newRAGC = 0;
uint8_t RAGCValues[10];
uint8_t *RAGCValues_p = &RAGCValues[0];
uint16_t MeanRAGC = 0;
uint8_t RAGCDataPoints = 0;

/*
 * Initialization of the automatic gain control. The resistance of the two 1k-100k digital potentiometers is set to 1k and thus the gain to 1.
 * The amount of the current gain is stored in the variable RAGC.
 */

void init_AGC(volatile uint32_t *TimingGain)
{
	 DDRD |= (1 << INC);
	 DDRB |= (1 << UD);

	 PORTD &= ~(1 << INC);
	 PORTB &= ~(1 << UD);

	 for(uint8_t i=0 ; i<100 ; i++){
		 *TimingGain = 0;
		 PORTD |= (1 << INC);
		 while((*TimingGain)<4);
		 PORTD &= ~(1 << INC);
		 while((*TimingGain)<8);
	 }
	 RAGC = 1;
}

/*
 * AGC(); is a function for controlling the automatic gain control. The minimum and maximum are detected first.
 * After evaluation of the peak-to-peak voltage, the required gain is calculated and, if necessary, the digital potentiometers are readjusted.
 */

void AGC(volatile uint32_t *TimingGain, volatile uint32_t *TimingEnvelope)
{
	  if((*TimingGain)>2000){
		  if((PPGIRACDCsig < PPGIRACDCmin)){
			  PPGIRACDCmin = PPGIRACDCsig;
			  *TimingEnvelope = 0;
		  }
		  if((PPGRACDCsig < PPGRACDCmin)){
			  PPGRACDCmin = PPGRACDCsig;
			  *TimingEnvelope = 0;
		  }

		  if((PPGIRACDCsig > PPGIRACDCmax)){
			  PPGIRACDCmax = PPGIRACDCsig;
			  *TimingEnvelope = 0;
		  }
		  if((PPGRACDCsig > PPGRACDCmax)){
			  PPGRACDCmax = PPGRACDCsig;
			  *TimingEnvelope = 0;
		  }
		  if((*TimingEnvelope) >= 200){
			
				if((PPGRACDCmax - PPGRACDCmin)>(PPGIRACDCmax - PPGIRACDCmin))
				{
					PPGpeakpeak = PPGRACDCmax - PPGRACDCmin;
				}
				else
				{
					PPGpeakpeak = PPGIRACDCmax - PPGIRACDCmin;
				}
			  
				newRAGC = ((400/(PPGpeakpeak)));
			  
				if(newRAGC>=100)
				{ 
					newRAGC = 100;
				}
				if(newRAGC<=1)
				{
					newRAGC = 1;
				}
			  
				MeanRAGC = 0;
			  
				if (RAGCDataPoints < 10)
				{
					RAGCDataPoints++;
				}

				for(uint8_t i=0 ; i<9 ; i++){
					RAGCValues[i] = RAGCValues[i+1];
					MeanRAGC += RAGCValues[i+1];
				}
				RAGCValues[9] = newRAGC;
				MeanRAGC += RAGCValues[9];
				MeanRAGC = MeanRAGC/RAGCDataPoints;
			  
			    if(MeanRAGC > RAGC)
				{
					PORTB |= (1 << UD);
					while(MeanRAGC > RAGC){
						*TimingGain = 0;
						PORTD |= (1 << INC);
						while((*TimingGain)<4);
						PORTD &= ~(1 << INC);
						while((*TimingGain)<8);
						RAGC++;
					}
				}
				else{
					PORTB &= ~(1 << UD);
					while(MeanRAGC < RAGC){
						*TimingGain = 0;
						PORTD |= (1 << INC);
						while((*TimingGain)<4);
						PORTD &= ~(1 << INC);
						while((*TimingGain)<8);
						RAGC-=1;
					}
				}

				PPGIRACDCmax = -1023;
				PPGIRACDCmin = 1023;
				PPGRACDCmax = -1023;
				PPGRACDCmin = 1023;
				
		  }
	  }
}