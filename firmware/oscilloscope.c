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
    ADCEnable();
}


inline int8_t
delta(uint16_t previous, uint16_t next)
{
    int16_t delta = next - previous;
    if (delta > 127)
        return 127;
    else if (delta < -127)
        return -127;
    else
        return (int8_t) delta;
}


void
OscilloscopeMain(tOscilloscope *self)
{
    uint32_t i;
    uint16_t *buffer0;
    uint16_t *buffer1;
    tPage *page0;
    tPage *page1;
    uint16_t state0, state1;
    int8_t *data0;
    int8_t *data1;

    if (!self->active) return;

    //while (self->active) {

    if (ADCReady())
    {
        buffer0 = ADCGetBuffer(0);
        buffer1 = ADCGetBuffer(1);

        ASSERT(!MemoryFull(&memory));
        page0 = MemoryAcquire(&memory);
        MemoryWrite(&memory);
        page1 = MemoryAcquire(&memory);
        MemoryWrite(&memory);

        page0->buffer[0] = startOscilloscope;
        page0->buffer[1] = ByteArray;
        page0->buffer[2] = 0;
        page0->buffer[3] = 0;

        state0 = buffer0[0] >> 2;
        *(uint16_t *) (page0->buffer + 4) = state0;

        data0 = (int8_t *) (page0->buffer + OSCILLOSCOPE_HEADER_LENGTH);

        page1->buffer[0] = startOscilloscope;
        page1->buffer[1] = ByteArray;
        page1->buffer[2] = 1;
        page1->buffer[3] = 0;

        state1 = buffer1[0] >> 2;
        *(uint16_t *) (page1->buffer + 4) = state1;

        data1 = (int8_t *) (page1->buffer + OSCILLOSCOPE_HEADER_LENGTH);

        //ASSERT(OSCILLOSCOPE_PACKET_LENGTH >= OSCILLOSCOPE_HEADER_LENGTH + ADC_BUFFER_LENGTH);

        for (i = 1; i < ADC_BUFFER_LENGTH; i++)
        {
            data0[i] = delta(state0, buffer0[i] >> 2);
            state0 += data0[i];
            data1[i] = delta(state1, buffer1[i] >> 2);
            state1 += data1[i];
        }

        ADCRelease();

        self->count++;

        if (self->count == MEMORY_LENGTH / 2)
        {
            page1->buffer[3] = 1; // mark this the last package
            ADCDisable();
            //DEBUG_PRINT("10th packet\n");
            //self->send = 1; // tell USB to deliver data
            MemoryStartSending(&memory, 0);
            self->active = 0;
        }

        //self->write = (self->write + 1) % OSCILLOSCOPE_QUEUE_LENGTH;
    }

    //}
}


void
OscilloscopeInit(tOscilloscope *self)
{
    self->active = 0;
}
