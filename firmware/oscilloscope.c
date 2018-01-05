/*
 * oscilloscope.c
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

#include <stdint.h>
#include <stdbool.h>
#include "driverlib/debug.h"
#include "oscilloscope.h"
#include "debug.h"
#include "adc.h"
#include "lenlab_protocol.h"
#include "memory.h"


tOscilloscope oscilloscope;


void
OscilloscopeStart(tOscilloscope *self)
{
    //DEBUG_PRINT("oscilloscope start\n");
    if (self->active)// || self->send)
        return;

    self->active = 1;
    self->count = 0;
    MemoryAllocate(&memory); // TODO MemoryFree call
    ADCEnable(&adc0);
}


void
OscilloscopeMain(tOscilloscope *self)
{
    uint32_t i;
    uint16_t *buffer;
    tPage *page;

    if (!self->active) return;

    if (adc0.lock)
    {
        buffer = ADCGetBuffer(&adc0);

        ASSERT(!MemoryFull(&memory));
        page = MemoryAcquire(&memory);

        page->buffer[0] = startOscilloscope;
        page->buffer[1] = ByteArray;
        page->buffer[2] = 1;
        page->buffer[3] = 0;

        //DEBUG_PRINT("adc ready %d\n", self->write);

        //ASSERT(OSCILLOSCOPE_PACKET_LENGTH >= OSCILLOSCOPE_HEADER_LENGTH + ADC_BUFFER_LENGTH);

        for (i = 0; i < ADC_BUFFER_LENGTH; i++)
        {
            page->buffer[OSCILLOSCOPE_HEADER_LENGTH + i] = buffer[i] >> 4;
        }

        ADCUnlock(&adc0);

        self->count++;

        if (self->count == 10)
        {
            page->buffer[3] = 1; // mark this the last package
            ADCDisable(&adc0);
            //DEBUG_PRINT("10th packet\n");
            //self->send = 1; // tell USB to deliver data
            MemoryStartSending(&memory, 0);
            self->active = 0;
        }

        MemoryWrite(&memory);
        //self->write = (self->write + 1) % OSCILLOSCOPE_QUEUE_LENGTH;
    }
}


void
OscilloscopeInit(tOscilloscope *self)
{
    self->active = 0;
}
