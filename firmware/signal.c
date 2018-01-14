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

#include <stdint.h>
#include <stdbool.h>
#include "driverlib/debug.h"
#include "signal.h"
#include "lenlab_protocol.h"
#include "debug.h"
#include "driverlib/systick.h"
#include "driverlib/sysctl.h"
#include "ssi.h"
#include "reply_handler.h"

tSignal signal;

inline int32_t
fixed(int32_t value)
{
    return value << 11;
}

inline int32_t
f_div(int32_t a, int32_t b)
{
    return (a << 11) / b;
}

inline int32_t
f_mul(int32_t a, int32_t b)
{
    return (a * b) >> 11;
}

// in fixed point 11 bit
#define f_PI (6434)
#define f_PI2 (3217)

inline int32_t
taylor(int32_t x)
{
    int32_t x2 = f_mul(x, x);
    int32_t x3 = f_mul(x2, x);
    int32_t x5 = f_mul(x3, x2);
    int32_t x7 = f_mul(x5, x2);

    return (x) - (x3 / 6) + (x5 / 120) - (x7 / 5040);
}

#define SIGNAL_OFFSET 2048
#define SIGNAL_MAX 4095

inline uint16_t
DACFormat(int32_t value, bool channel)
{
    value += SIGNAL_OFFSET;
    value = value > 0 ? value : 0;
    value = value < SIGNAL_MAX ? value : SIGNAL_MAX;
    value |= channel ? 0x3000 : 0xB000;
    return (uint16_t) value;
}

void
WriteSine(uint16_t *buffer, uint32_t periods)
{
    int32_t value;
    int32_t i;
    int32_t x;

    bool sign;

    for (i = 0; i < 500; i++)
    {
        x = periods * i * f_PI / 250;
        sign = 0;
        while (x > f_PI) {
            x -= f_PI;
            sign = !sign;
        }

        value = (sign ? -1 : 1) * taylor(x - f_PI2);

        buffer[2*i] = DACFormat(value, 0);
        buffer[2*i + 1] = DACFormat(value / 2, 1);
    }
}

void
SignalSetSine(uint8_t multiplier, uint8_t predivisor, uint8_t divisor)
{
    WriteSine(SSIGetBuffer(), multiplier);
    SSISetDivider(predivisor, divisor);
}

void
SignalStart(void)
{
    SSIStart();
}

void
SignalInit(void)
{
    SSISetLength(1000);
    SignalSetSine(1, 2, 5);
}
