/*
 * signal.c
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

#include "signal.h"
#include "data_handler.h"
#include "lenlab_protocol.h"
#include "debug.h"
#include "driverlib/systick.h"

inline int32_t
f_mul(int32_t a, int32_t b)
{
    return (a * b) >> 11;
}

inline int32_t
taylor(int32_t x)
{
    int32_t x2 = f_mul(x, x);
    int32_t x3 = f_mul(x2, x);
    int32_t x5 = f_mul(x3, x2);
    int32_t x7 = f_mul(x5, x2);

    return (x) - (x3 / 6) + (x5 / 120) - (x7 / 5040);
}

void
SignalCalculateSine(void)
{
    // 1.2 ms mit 4 mal taylor
    // 0.5 ms mit 1 mal taylor und kopieren
    uint32_t i;
    tDataEvent *event = DataQueueAcquire(&data_handler.data_queue);
    int16_t *buffer = (int16_t *) (event->payload + 8);
    volatile uint32_t start = SysTickValueGet();
    volatile uint32_t time;
    DEBUG_PRINT("start %u", start);
    // fixed point arithmetic (2^11)F = 1
    // -> pi/2 = (3217)F
    // about 512 steps for one period, 128 for pi/2, (26)F per step
    // so 124 steps for pi/2
    for (i = 0; i < 124; i++)
        buffer[i] = taylor(i*26);
    /*
    for (i = 0; i < 124; i++)
        buffer[124+i] = taylor((124-i)*26);
    for (i = 0; i < 124; i++)
        buffer[248+i] = -taylor(i*26);
    for (i = 0; i < 124; i++)
        buffer[248+124+i] = -taylor((124-i)*26);
    */
    for (i = 0; i < 124; i++)
        buffer[124+i] = buffer[123-i];
    for (i = 0; i < 124; i++)
        buffer[248+i] = -buffer[i];
    for (i = 0; i < 124; i++)
        buffer[248+124+i] = -buffer[123-i];

    event->payload[0] = calculateSine;
    event->payload[1] = Int16;
    event->length = 4+4+2*495;
    time = SysTickValueGet();
    *((uint32_t *) (event->payload + 4)) = start - time; // der timer zählt runter!
    DEBUG_PRINT("done %u", time);
    DataQueueWrite(&data_handler.data_queue);
}
