/*
 * oscilloscope.h
 *
 */

#ifndef OSCILLOSCOPE_H_
#define OSCILLOSCOPE_H_


#include "osc_seq.h"


#define OSCILLOSCOPE_MEMORY_LENGTH 22


typedef struct Oscilloscope {

    tADCGroup *adc_group;
    tOscSeqGroup seq_group;

    unsigned char lock;

    tPage memory[OSCILLOSCOPE_MEMORY_LENGTH];

} tOscilloscope;


extern tOscilloscope oscilloscope;


tError OscilloscopeStart(tOscilloscope *self, uint32_t samplerate);

tError OscilloscopeStop(tOscilloscope *self);

void OscilloscopeMain(tOscilloscope *self);

void OscilloscopeInit(tOscilloscope *self, tADCGroup *adc_group);


#endif /* OSCILLOSCOPE_H_ */
