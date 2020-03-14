/*
 * LEDControl.c
 *
 * Created: 04.04.2018 11:49:26
 *  Author: Moritz Klimt
 */

#include <avr/io.h>
#include <stdint.h>
#include "Update_Signals.h"

#define PWMR OCR1A
#define PWMIR OCR1B

int FingerIn_Out()
{
	if((IRDC > 1000)&&(RDC > 1000))
	{
		PWMIR = 675;
		PWMR  = 675;
		return 0;
	}
	else
	{
		return 1;
	}
}

/*
 *  LEDBrightnessUP(); is a function to increase the LED brightness.
 *  The address of the GainDelay variable is passed to it via a pointer
 *  in order to delay the control slightly via the value stored at this address.
 *  If the value of the DC signals is below 600 (approx. 3V),
 *  the PWM duty cycle is increased until the DC signal is at 3V or the PWM duty cycle of 100% is reached.
 */

void LEDBrightnessUP(uint32_t *Timing)
{
	*Timing = 0;
	while(((PPGRDCsig) < 600)&&(FingerIn_Out()==1)){
		if(((*Timing) >= 200)&&(PWMR < 4094)){
			if (PPGRDCsig<500)
			{
				PWMR+=2;
				*Timing = 0;
			}
			else
			{
				PWMR++;
				*Timing = 0;
			}
		}
		if(PWMR>=4094){
			break;
		}
	}
	while(((PPGIRDCsig) < 600)&&(FingerIn_Out()==1)){
		if(((*Timing) >= 200)&&(PWMIR < 4094)){
			if (PPGIRDCsig<500)
			{
				PWMIR+=2;
				*Timing = 0;
			}
			else
			{
				PWMIR++;
				*Timing = 0;
			}
		}
		if(PWMIR>=4094){
			break;
		}
	}
}

/*
 *  LEDBrightnessDOWN(); is a function to decrease the LED brightness.
 *  The address of the GainDelay variable is passed to it via a pointer
 *  in order to delay the control slightly via the value stored at this address.
 *  If the value of the DC signals is above 600 (approx. 3V),
 *  the PWM duty cycle is decreased until the DC signal is at 3V or the PWM duty cycle of 6,5% is reached.
 */

void LEDBrightnessDOWN(uint32_t *Timing)
{
	*Timing = 0;
	while(((PPGRDCsig) >= 600)&&(FingerIn_Out()==1)){
		if(((*Timing) >= 200)&&(PWMR > 635)){
			if (PPGRDCsig>700)
			{
				PWMR -=2;
				*Timing = 0;
			}
			else
			{
				PWMR -= 1;
				*Timing = 0;
			}
		}
		if(PWMR<=635){
			break;
		}
	}
	while(((PPGIRDCsig) >= 600)&&(FingerIn_Out()==1)){
		if(((*Timing) >= 200)&&(PWMIR > 635)){
			if (PPGIRDCsig>700)
			{
				PWMIR -=2;
				*Timing = 0;
			}
			else
			{
				PWMIR -= 1;
				*Timing = 0;
			}
		}
		if(PWMIR<=635){
			break;
		}
	}
}
