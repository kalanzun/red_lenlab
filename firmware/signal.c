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

#include "signal.h"

#include "ssi.h"


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
DACFormat(int32_t value, unsigned char channel)
{
    value += SIGNAL_OFFSET;
    value = value > 0 ? value : 0;
    value = value < SIGNAL_MAX ? value : SIGNAL_MAX;
    value |= channel ? 0xB000 : 0x3000;
    return (uint16_t) value;
}


void
WriteSine(uint16_t *buffer, uint32_t multiplier, uint32_t amplitude, uint32_t second)
{
    int32_t value;
    int32_t i;
    int32_t x;

    unsigned char sign;

    for (i = 0; i < 500; i++)
    {
        x = multiplier * i * f_PI / 250;
        sign = 0;
        while (x > f_PI) {
            x -= f_PI;
            sign = !sign;
        }

        value = (sign ? -1 : 1) * f_mul(taylor(x - f_PI2), amplitude);

        buffer[2*i] = DACFormat(value, 0);
    }

    value = 0;

    for (i = 0; i < 500; i++)
    {
        if (second)
        {
            x = multiplier * second * i * f_PI / 250;
            sign = 0;
            while (x > f_PI) {
                x -= f_PI;
                sign = !sign;
            }

            value = (sign ? -1 : 1) * f_mul(taylor(x - f_PI2), amplitude);
        }

        buffer[2*i + 1] = DACFormat(value, 1);
    }
}


void
SignalSetSine(tSignal *self, uint32_t multiplier, uint32_t predivider, uint32_t divider, uint32_t amplitude, uint32_t second)
{
    // unconditionally overwrite the SSI DMA buffer
    WriteSine(SSIGetBuffer(&ssi), multiplier, amplitude, second);
    SSISetDivider(&ssi, predivider, divider);
}


tError
SignalStart(tSignal *self)
{
    if (self->lock) return LOCK_ERROR;

    SSIStart(&ssi);

    self->lock = true;

    return OK;
}


tError
SignalStop(tSignal *self)
{
    if (!self->lock) return LOCK_ERROR;

    SSIStop(&ssi);

    self->lock = false;

    return OK;
}


void
SignalInit(tSignal *self)
{
    self->lock = false;

    // fixed length SSI buffer
    SSISetLength(&ssi, 1000);
}
