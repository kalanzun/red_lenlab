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
#include "usb_device.h"


tOscilloscope oscilloscope;



void
OscilloscopeStart(tOscilloscope *self, uint32_t samplerate)
{
    DEBUG_PRINT("OscilloscopeStart %d\n", samplerate);

    if (self->active)// || self->send)
        return;

}


void
OscilloscopeStartTrigger(tOscilloscope *self, uint32_t samplerate)
{
    uint32_t i;

    DEBUG_PRINT("OscilloscopeStartTrigger %d\n", samplerate);

    if (self->active)// || self->send)
        return;

}


void
OscilloscopeMain(tOscilloscope *self)
{

    if (!self->active)
        return;

}


void
OscilloscopeInit(tOscilloscope *self)
{
    self->active = 0;
}
