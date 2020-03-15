/*
 * Update_Signals.h
 *
 * Created: 12.03.2018 17:28:53
 *  Author: Moritz Klimt
 */


#ifndef UPDATE_SIGNALS_H_
#define UPDATE_SIGNALS_H_

extern volatile int32_t PPGIRsig;
extern volatile int32_t PPGRsig;
extern volatile int32_t PPGIRDCsig;
extern volatile int32_t PPGRDCsig;
extern volatile int32_t PPGIRACDCsig;
extern volatile int32_t PPGRACDCsig;
extern volatile int16_t IRAC;
extern volatile int16_t RAC;
extern volatile int16_t IRDC;
extern volatile int16_t RDC;



extern void UpdateSignals();


#endif /* UPDATE_SIGNALS_H_ */
