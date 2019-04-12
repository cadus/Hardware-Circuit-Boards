/*
 * calculatingSPO2.c
 *
 * Created: 04.04.2018 20:04:46
 *  Author: Moritz Klimt
 */ 
#include <stdint.h>
#include <math.h>

float aACIR = 0.0;
float aACR = 0.0;
float R = 0.0;
float R_SPO2 = 0.0;
int16_t SPO2Values[10];
int16_t *SPO2Values_p = &SPO2Values[0];
int16_t MeanSPO2 = 0;
uint8_t SPO2DataPoint = 0;
uint8_t SPO2DataPoints = 0;

uint8_t calculatingSPO2(int16_t *DataIR_p, int16_t *DataR_p, int16_t Buffersize)
{
	for(uint8_t i = 0; i<Buffersize ; i++)
	{
		aACIR += (*(DataIR_p+i)*(*(DataIR_p+i))*20);
		aACR += (*(DataR_p+i)*(*(DataR_p+i))*20);
	}
	aACIR = aACIR/(Buffersize*20);
	aACR = aACR/(Buffersize*20);
	aACIR = sqrt(aACIR);
	aACR = sqrt(aACR);
	R = (aACR/aACIR);
	if((R>0)&&(R<5))
	{
		R_SPO2 = -18.98*(R*R)+7.811*R+98.8;					//Sp02 Calibration Curve
		SPO2DataPoint = 1;
	}
	aACIR = 0;
	aACR = 0;

	if(SPO2DataPoint == 1){
		MeanSPO2 = 0;
		if(SPO2DataPoints < 10){
			SPO2DataPoints++;
		}
		for(uint8_t i=0 ; i<9 ; i++){
			SPO2Values[i] = SPO2Values[i+1];
			MeanSPO2 += SPO2Values[i+1];
		}
		SPO2Values[9] = (int16_t)(R_SPO2);
		MeanSPO2 += SPO2Values[9];
		MeanSPO2 = MeanSPO2/SPO2DataPoints;
		SPO2DataPoint = 0;
	}
	return MeanSPO2;
}