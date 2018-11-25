/*
 * logger.c
 *

Lenlab, an oscilloscope software for the TI LaunchPad EK-TM4C123GXL
Copyright (C) 2017 Christoph Simon and the Lenlab developer team

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.

*/

#include "logger.h"

#include "reply_handler.h"


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


tError
LoggerStart(tLogger *self, uint32_t interval)
{
    if (self->lock) return LOCK_ERROR;

    if (self->adc_group->lock) return STATE_ERROR;

    ADCGroupSetHardwareOversample(self->adc_group, 1);

    LogSeqGroupEnable(&self->seq_group, interval);

    ADCGroupLock(self->adc_group);

    self->lock = 1;

    return OK;
}


tError
LoggerStop(tLogger *self)
{
    if (!self->lock) return LOCK_ERROR;

    LogSeqGroupDisable(&self->seq_group);

    self->lock = 0;

    ADCGroupUnlock(self->adc_group);

    return OK;
}


void
LoggerMain(tLogger *self)
{
    uint32_t size;
    tEvent *reply;

    if (!self->lock) return;

    if (LogSeqGroupError(&self->seq_group)) {

        LoggerStop(self);

        ASSERT(0); // TODO Logger error handling in release build

    }

    else if (LogSeqGroupReady(&self->seq_group)) {

        //DEBUG_PRINT("LoggerData\n");

        reply = QueueAcquire(&reply_handler.reply_queue);

        EventSetReply(reply, LoggerData);
        EventSetType(reply, IntArray);

        size = sizeof(uint32_t) * ADC_GROUP_SIZE;
        ASSERT(size <= LENLAB_PACKET_BODY_LENGTH);
        EventSetBodyLength(reply, size);

        size = LogSeqGroupDataGet(&self->seq_group, (uint32_t *) (EventGetBody(reply)));
        ASSERT(size == ADC_GROUP_SIZE);

        QueueWrite(&reply_handler.reply_queue);

        LogSeqGroupRelease(&self->seq_group);

    }
}


void
LoggerInit(tLogger *self, tADCGroup *adc_group)
{
    self->adc_group = adc_group;

    LogSeqGroupInit(&self->seq_group, adc_group);

    self->lock = 0;
}
