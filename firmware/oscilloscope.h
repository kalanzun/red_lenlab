/*
 * oscilloscope.h
 *
 */

#ifndef OSCILLOSCOPE_H_
#define OSCILLOSCOPE_H_


#include "osc_seq.h"


typedef struct Oscilloscope {

    bool lock;

} tOscilloscope;


extern tOscilloscope oscilloscope;


tError OscilloscopeStart(tOscilloscope *self, uint32_t samplerate);

tError OscilloscopeStop(tOscilloscope *self);

void OscilloscopeMain(tOscilloscope *self);

void OscilloscopeInit(tOscilloscope *self);


#endif /* OSCILLOSCOPE_H_ */
