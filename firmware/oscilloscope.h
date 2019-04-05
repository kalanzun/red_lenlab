/*
 * oscilloscope.h
 *
 */

#ifndef OSCILLOSCOPE_H_
#define OSCILLOSCOPE_H_


#include "osc_seq.h"
#include "memory.h"


typedef struct Oscilloscope {

    tADCGroup *adc_group;
    tOscSeqGroup seq_group;

    unsigned char lock;

} tOscilloscope;


extern tOscilloscope oscilloscope;


tError OscilloscopeStart(tOscilloscope *self, uint32_t samplerate);

tError OscilloscopeStop(tOscilloscope *self);

void OscilloscopeMain(tOscilloscope *self);

void OscilloscopeInit(tOscilloscope *self, tADCGroup *adc_group);


#endif /* OSCILLOSCOPE_H_ */
