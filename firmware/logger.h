/*
 * logger.h
 *
 */

#ifndef LOGGER_H_
#define LOGGER_H_


#include "error.h"
#include "log_seq.h"


typedef struct Logger {

    tADCGroup *adc_group;
    tLogSeqGroup seq_group;

    bool lock;

} tLogger;


extern tLogger logger;


tError LoggerStart(tLogger *self, uint32_t interval);


tError LoggerStop(tLogger *self);


void LoggerInit(tLogger *self, tADCGroup *adc_group);


#endif /* LOGGER_H_ */
