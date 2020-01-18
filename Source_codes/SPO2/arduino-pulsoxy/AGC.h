/*
 * AGC.h
 *
 * Created: 04.04.2018 15:22:32
 *  Author: Moritz Klimt
 */ 

extern uint8_t RAGC;
extern uint8_t *RAGCValues_p;
extern uint8_t RAGCDataPoints;


#ifndef AGC_H_
#define AGC_H_

void init_AGC(volatile uint32_t *TimingGain);
void AGC(volatile uint32_t *TimingGain, volatile uint32_t *TimingEnvelope);



#endif /* AGC_H_ */
