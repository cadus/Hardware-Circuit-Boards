/*
 * Update_Signals.c
 *
 * Created: 12.03.2018 17:28:43
 *  Author: Moritz Klimt
 */ 
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "ADC.h"

/*
 * Variables for updating the AC and DC-PPG Signals 
 */

#define PPGIRDC 0
#define PPGRDC 1
#define PPGIR 2
#define PPGR 3

volatile int16_t IRAC = 0;
volatile int16_t RAC = 0;
volatile int16_t IRDC = 0;
volatile int16_t RDC = 0;
volatile int32_t PPGIRsig = 0;
volatile int32_t PPGRsig = 0;
volatile int32_t PPGIRDCsig = 0;
volatile int32_t PPGRDCsig = 0;
volatile int32_t PPGIRACDCsig = 0;
volatile int32_t PPGRACDCsig = 0;

void UpdateSignals()
{
	IRAC = ADCread(PPGIR);											//ADC of the AC-IR Signal
	RAC = ADCread(PPGR);											//ADC of the AC-R Signal
	IRDC = ADCread(PPGIRDC);										//ADC of the DC-IR Signal
	RDC = ADCread(PPGRDC);											//ADC of the DC-R Signal
	PPGIRsig = (((115*PPGIRsig)>>7)+((13*(IRAC-512))>>7));			//5 Hz low-pass filtering and removing the 2.5 V offset from the amplified AC-IR Signal
	PPGRsig = (((115*PPGRsig)>>7)+((13*(RAC-512))>>7));				//5 Hz low-pass filtering and removing the 2.5 V offset from the amplified AC-R Signal
	PPGIRDCsig = (((126*PPGIRDCsig)>>7)+((2*IRDC)>>7));				//0.5 Hz low-pass filtering  the DC-IR Signal
	PPGRDCsig = (((126*PPGRDCsig)>>7)+((2*RDC)>>7));				//0.5 Hz low-pass filtering  the DC-R Signal
	PPGIRACDCsig = (IRDC-PPGIRDCsig);								//0.5 Hz high-pass filtered AC-IR Signal on top of the DC-IR Signal without amplification
	PPGRACDCsig = (RDC-PPGRDCsig); 									//0.5 Hz high-pass filtered AC-R Signal on top of the DC-R Signal without amplification
}