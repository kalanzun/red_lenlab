/*
 * logger.h
 *
 */

#ifndef LOGGER_H_
#define LOGGER_H_


#include "log_seq.h"


typedef struct Logger {

    tADCGroup *adc_group;
    tLogSeqGroup seq_group;

} tLogger;


extern tLogger logger;


inline void
LoggerInit(tLogger *self, tADCGroup *adc_group)
{
    self->adc_group = adc_group;

    LogSeqGroupInit(&self->seq_group, adc_group);
}


#endif /* LOGGER_H_ */
