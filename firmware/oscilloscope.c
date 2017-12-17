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


tOscilloscope oscilloscope;


void
OscilloscopeStart(tOscilloscope *self)
{
    //DEBUG_PRINT("oscilloscope start\n");
    if (self->active || self->send)
        return;

    self->active = 1;
    ADCEnable(&adc0);
}


void
OscilloscopeMain(tOscilloscope *self)
{
    uint32_t i;
    uint16_t *buffer;

    if (!self->active) return;

    if (adc0.lock)
    {
        self->queue[self->write][0] = startOscilloscope;
        self->queue[self->write][1] = Int8;
        self->queue[self->write][2] = 1;
        self->queue[self->write][3] = 0;

        //DEBUG_PRINT("adc ready %d\n", self->write);

        buffer = ADCGetBuffer(&adc0);

        ASSERT(OSCILLOSCOPE_PACKET_LENGTH >= OSCILLOSCOPE_HEADER_LENGTH + ADC_BUFFER_LENGTH);

        for (i = 0; i < ADC_BUFFER_LENGTH; i++)
        {
            self->queue[self->write][OSCILLOSCOPE_HEADER_LENGTH + i] = buffer[i] >> 4;
        }

        ADCUnlock(&adc0);

        if (self->write + 1 == OSCILLOSCOPE_QUEUE_LENGTH)
        {
            self->queue[self->write][3] = 1; // mark this the last package
            ADCDisable(&adc0);
            self->send = 1; // tell USB to deliver data
            self->active = 0;
        }
        self->write = (self->write + 1) % OSCILLOSCOPE_QUEUE_LENGTH;
    }
}


void
OscilloscopeInit(tOscilloscope *self)
{
    self->active = 0;
    self->send = 0;
    self->read = 0;
    self->write = 0;
}
