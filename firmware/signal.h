/*
 * signal.h
 *
 */

#ifndef SIGNAL_H_
#define SIGNAL_H_


#include "error.h"


typedef struct Signal {
    bool lock;
} tSignal;


extern tSignal signal;


void SignalSetSine(tSignal *self, uint32_t multiplier, uint32_t predivider, uint32_t divider, uint32_t amplitude, uint32_t second);

tError SignalStart(tSignal *self);

tError SignalStop(tSignal *self);

void SignalInit(tSignal *self);


#endif /* SIGNAL_H_ */
