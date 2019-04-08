/*
 * oscilloscope.h
 *
 */

#ifndef OSCILLOSCOPE_H_
#define OSCILLOSCOPE_H_


#include "osc_seq.h"
#include "memory.h"


typedef struct Oscilloscope {
    tOscSeqGroup seq_group;

    tMemory *memory;
    tADCGroup *adc_group;

    bool lock;
} tOscilloscope;


extern tOscilloscope oscilloscope;


tError OscilloscopeStart(tOscilloscope *self, uint32_t samplerate);

tError OscilloscopeStop(tOscilloscope *self);

void OscilloscopeMain(tOscilloscope *self);

void OscilloscopeInit(tOscilloscope *self, tMemory *memory, tADCGroup *adc_group);


#endif /* OSCILLOSCOPE_H_ */
