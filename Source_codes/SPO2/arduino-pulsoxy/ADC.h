/*
 * ADC.h
 *
 * Created: 07.03.2018 16:02:54
 *  Author: Moritz Klimt
 */ 



#ifndef ADC_H_
#define ADC_H_

void init_ADC();
uint16_t ADCread(uint8_t adcpin);

#endif /* ADC_H_ */