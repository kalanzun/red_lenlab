/*
 * logger.c
 *
 */

#include "logger.h"


void
ADC0SS3Handler(void)
{
    tLogSeq *self = &logger.seq_group.log_seq[0];

    LogSeqIntHandler(self);
}


void
ADC1SS3Handler(void)
{
    tLogSeq *self = &logger.seq_group.log_seq[1];

    LogSeqIntHandler(self);
}
