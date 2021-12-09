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

#include "oscilloscope.h"

#include "driverlib/debug.h"

#include "reply_handler.h"


struct Oscilloscope oscilloscope;


void
OscilloscopeStart(uint32_t log2oversamples)
{
    ADCGroupSetHardwareOversample(log2oversamples);

    OscSeqGroupEnable();

    oscilloscope.active = true;
}


void
OscilloscopeMain(void)
{
    struct OscSeq *osc;

    struct OscData *page;
    struct Message *reply;

    if (!oscilloscope.active) return;

    if (OscSeqGroupReady()) {
        // ADC and uDMA are finished
        OscSeqGroupDisable();
        oscilloscope.active = false;

        FOREACH_OSC {
            for (page = (struct OscData *) osc->page_queue->array; page != (struct OscData *) osc->page_queue->array + osc->page_queue->length; ++page) {
                setReply((struct Message *) page, OscilloscopeData, ShortArray, 0);
            }

            reply = (struct Message *) RingAcquire(&reply_queue);
            setPageQueue(reply, osc->page_queue);
            RingWrite(&reply_queue);
        }
    }
}


void
OscilloscopeInit(void)
{
    OscSeqGroupInit();

    oscilloscope.active = false;
}
