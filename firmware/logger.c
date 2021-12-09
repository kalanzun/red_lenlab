/*
 * Lenlab, an oscilloscope software for the TI LaunchPad EK-TM4C123GXL
 * Copyright (C) 2017-2021 Christoph Simon and the Lenlab developer team
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <stdbool.h>
#include <stdint.h>

#include "logger.h"

#include "log_seq.h"
#include "message.h"
#include "reply_handler.h"


void
LoggerStart(uint32_t interval)
{
    ADCGroupSetHardwareOversample(5); // oversampling factor 32

    LogSeqGroupEnable(interval);
}


void
LoggerStop(void)
{
    LogSeqGroupDisable();
}


void
LoggerMain(void)
{
    int count;
    struct Message *reply;

    if (LogSeqGroupError()) {
        LoggerStop();

        ASSERT(0); // TODO Logger error handling in release build
        return;
    }

    if (LogSeqGroupReady() && reply_queue.has_space) {
        reply = (struct Message *) RingAcquire(&reply_queue);
        setReply(reply, Log, IntArray, 0); // TODO logger.reference
        reply->size = 4 + 5 * sizeof(uint32_t); // time and 4 channels

        ASSERT(log_seq_group.log_seq[0].count == log_seq_group.log_seq[1].count);
        setInt(reply, 0, log_seq_group.log_seq[0].count);

        count = LogSeqGroupDataGet((uint32_t *) &reply->body + 1);
        ASSERT(count == 4); // we might have two entries as well

        LogSeqGroupRelease();
        RingWrite(&reply_queue);
    }
}


void
LoggerInit(void)
{
    LogSeqGroupInit();
}
