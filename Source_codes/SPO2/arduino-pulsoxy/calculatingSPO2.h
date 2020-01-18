/*
 * calculatingSPO2.h
 *
 * Created: 04.04.2018 20:05:12
 *  Author: Moritz Klimt
 */ 

extern uint8_t SPO2DataPoints;
extern int16_t *SPO2Values_p;

#ifndef CALCULATINGSPO2_H_
#define CALCULATINGSPO2_H_


uint8_t calculatingSPO2(volatile int16_t *DataIR_p, volatile int16_t *DataR_p, int16_t Buffersize);


#endif /* CALCULATINGSPO2_H_ */
