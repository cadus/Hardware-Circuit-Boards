/*
 * LEDControl.h
 *
 * Created: 04.04.2018 11:49:49
 *  Author: Moritz Klimt
 */ 


#ifndef LEDCONTROL_H_
#define LEDCONTROL_H_


int FingerIn_Out();
void LEDBrightnessUP(volatile uint32_t *Timing);
void LEDBrightnessDOWN(volatile uint32_t *Timing);



#endif /* LEDBRIGHTNESSCONTROL_H_ */